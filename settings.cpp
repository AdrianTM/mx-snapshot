/**********************************************************************
 *  settings.cpp
 **********************************************************************
 * Copyright (C) 2020 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of MX Snapshot.
 *
 * MX Snapshot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MX Snapshot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MX Snapshot.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "settings.h"

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QRegularExpression>
#include <QSettings>

#ifndef CLI_BUILD
#include <QMessageBox>
#endif

Settings::Settings(const QCommandLineParser &arg_parser)
{
    if (QFileInfo::exists(
            QStringLiteral("/tmp/installed-to-live/cleanup.conf"))) // cleanup installed-to-live from other sessions
        QProcess::execute("installed-to-live", {"cleanup"});
    shell = new Cmd;

    loadConfig(); // load settings from .conf file
    setVariables();
    processArgs(arg_parser);
    if (arg_parser.isSet(QStringLiteral("month")))
        setMonthlySnapshot(arg_parser);
    override_size = arg_parser.isSet(QStringLiteral("override-size"));
    cli_mode = arg_parser.isSet(QStringLiteral("cli"));
    processExclArgs(arg_parser);
}

Settings::~Settings() = default;

// check if compression is available in the kernel (lz4, lzo, xz)
bool Settings::checkCompression() const
{
    if (compression == QLatin1String("gzip")) // don't check for gzip
        return true;
    if (!QFileInfo::exists("/boot/config-" + kernel)) // return true if cannot check config file
        return true;
    return (shell->run("grep ^CONFIG_SQUASHFS_" + compression.toUpper() + "=y /boot/config-" + kernel));
}

// adds or removes exclusion to the exclusion string
void Settings::addRemoveExclusion(bool add, QString exclusion)
{
    if (exclusion.startsWith(QLatin1String("/")))
        exclusion.remove(0, 1); // remove preceding slash
    if (add) {
        if (session_excludes.isEmpty())
            session_excludes.append("-e \"" + exclusion + "\"");
        else
            session_excludes.append(" \"" + exclusion + "\"");
    } else {
        session_excludes.remove(" \"" + exclusion + "\"");
        if (session_excludes == QLatin1String("-e"))
            session_excludes = QLatin1String("");
    }
}

bool Settings::checkSnapshotDir() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (!QDir().mkpath(snapshot_dir)) {
        qDebug() << QObject::tr("Could not create working directory. ") + snapshot_dir;
        return false;
    }
    shell->run("chown $(logname):$(logname) \"/" + snapshot_dir + "\"", false);
    return true;
}

bool Settings::checkTempDir()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Set workdir location if not defined in .conf file, doesn't exist, or not supported partition
    if (tempdir_parent.isEmpty() || !QFile::exists(tempdir_parent) || !isOnSupportedPart(tempdir_parent)) {
        tempdir_parent = snapshot_dir;
        if (!isOnSupportedPart(
                snapshot_dir)) // if not saving snapshot on a supported partition, put working dir in /tmp or /home
            tempdir_parent = largerFreeSpace(QStringLiteral("/tmp"), QStringLiteral("/home"));
        else
            tempdir_parent = largerFreeSpace(QStringLiteral("/tmp"), QStringLiteral("/home"), snapshot_dir);
    }

    tmpdir.reset(new QTemporaryDir(tempdir_parent + "/mx-snapshot-XXXXXXXX"));
    if (!tmpdir->isValid()) {
        qDebug() << QObject::tr("Could not create temp directory. ") + tmpdir.data()->path();
        return false;
    }
    work_dir = tmpdir.data()->path();
    free_space_work = getFreeSpace(work_dir);

    QDir().mkpath(work_dir + "/iso-template/antiX");
    qDebug() << "Work directory is placed in" << tempdir_parent;
    return true;
}

QString Settings::getEditor() const
{
    QString editor = gui_editor;
    if (editor.isEmpty()
        || QProcess::execute("/bin/bash", {"-c", "command -v " + editor})
               != 0) { // if specified editor doesn't exist get the default one
        QString local = QDir::homePath() + "/.local/share/applications ";
        if (!QFile::exists(local))
            local = QLatin1String("");
        QString desktop_file = shell->getCmdOut(
            "find " + local + "/usr/share/applications -name $(xdg-mime query default text/plain) -print -quit", true);
        editor = shell->getCmdOut("grep -m1 ^Exec= " + desktop_file, true);
        editor = editor.remove(QRegularExpression(QStringLiteral("^Exec=|%u|%U|%f|%F|%c|%C"))).trimmed();
        if (editor.isEmpty()) // if default one doesn't exit use nano as backup editor
            editor = QStringLiteral("x-terminal-emulator -e nano");
    }
    if (editor.endsWith(QLatin1String("kate")) || editor.endsWith(QLatin1String("kwrite"))
        || editor.endsWith(QLatin1String("atom"))) // need to run these as normal user
        editor = "runuser -u $(logname) " + editor;
    return editor;
}

// return the size of the snapshot folder
QString Settings::getSnapshotSize() const
{
    if (QFileInfo::exists(snapshot_dir)) {
        QString cmd = QStringLiteral(
                          "find \"%1\" -maxdepth 1 -type f -name '*.iso' -exec du -shc {} + |tail -1 |awk '{print $1}'")
                          .arg(snapshot_dir);
        auto size = shell->getCmdOut(cmd);
        if (!size.isEmpty())
            return size;
    }
    return QStringLiteral("0");
}

// return number of snapshots in snapshot_dir
int Settings::getSnapshotCount() const
{
    if (QFileInfo::exists(snapshot_dir)) {
        QFileInfoList list = QDir(snapshot_dir).entryInfoList(QStringList(QStringLiteral("*.iso")), QDir::Files);
        return list.size();
    }
    return 0;
}

quint64 Settings::getFreeSpace(const QString &path) const
{
    bool ok = false;
    quint64 result {};
    if (shell->getCmdOut("stat --file-system --format=%T \"" + path + "\"").trimmed() == "ramfs")
        result = shell->getCmdOut("LC_ALL=C free |awk '/^Mem/ {print $7}'").toULongLong(&ok);
    else
        result = shell->getCmdOut(QStringLiteral("df -k --output=avail \"%1\" |tail -n1").arg(path)).toULongLong(&ok);
    if (!ok) {
        qDebug() << "Can't calculate free space on" << path;
        return 0;
    }
    return result;
}

// return the XDG User Directory for each user with different localizations than English
QString Settings::getXdgUserDirs(const QString &folder)
{
    QString result;
    for (const QString &user : qAsConst(users)) {
        QString dir;
        bool success = shell->run("runuser " + user + " -c \"xdg-user-dir " + folder + "\"", dir);
        if (success) {
            if (englishDirs.value(folder) == dir.section(QStringLiteral("/"), -1) || dir.trimmed() == "/home/" + user
                || dir.trimmed() == "/home/" + user + "/"
                || dir.isEmpty()) // skip if English name or of return folder is the home folder (if XDG-USER-DIR not
                                  // defined)
                continue;
            if (dir.startsWith(QLatin1String("/")))
                dir.remove(0, 1); // remove training slash
            (folder == QLatin1String("DESKTOP")) ? dir.append("/!(minstall.desktop)")
                                                 : dir.append("/*\" \"" + dir + "/.*");
            (result.isEmpty()) ? result.append("\" \"" + dir) : result.append(" \"" + dir);
            result.append("\""); // close the quote for each user, will strip the last one before returning;
        }
    }
    result.chop(1); // chop the last quote, will be added later on in addRemoveExclusion
    return result;
}

void Settings::selectKernel()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    kernel.remove(
        QRegularExpression(QStringLiteral("^/boot/vmlinuz-"))); // remove path and part of name if passed as arg
    if (kernel.isEmpty()
        || !QFileInfo::exists("/boot/vmlinuz-" + kernel)) { // if kernel version not passed as arg, or incorrect
        kernel = current_kernel;
        if (!QFileInfo::exists(
                "/boot/vmlinuz-"
                + kernel)) { // if current kernel doesn't exist for some reason (e.g. WSL) in /boot pick latest kernel
            kernel = shell->getCmdOut(QStringLiteral("ls -1 /boot/vmlinuz-* |sort |tail -n1"))
                         .remove(QRegularExpression(QStringLiteral("^/boot/vmlinuz-")));
            if (!QFileInfo::exists("/boot/vmlinuz-" + kernel)) {
                QString message = QObject::tr("Could not find a usable kernel");
                if (qApp->metaObject()->className() != QLatin1String("QApplication"))
                    qDebug().noquote() << message;
#ifndef CLI_BUILD
                else
                    QMessageBox::critical(nullptr, QObject::tr("Error"), message);
#endif
                exit(EXIT_FAILURE);
            }
        }
    }
    // Check if SQUASHFS is available
    if (QProcess::execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", "/boot/config-" + kernel}) != 0) {
        QString message = QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
        if (qApp->metaObject()->className() != QLatin1String("QApplication"))
            qDebug().noquote() << message;
#ifndef CLI_BUILD
        else
            QMessageBox::critical(nullptr, QObject::tr("Error"), message);
#endif
        exit(EXIT_FAILURE);
    }
}

void Settings::setVariables()
{
    englishDirs = {
        {"DOCUMENTS", "Documents"}, {"DOWNLOAD", "Downloads"}, {"DESKTOP", "Desktop"},
        {"MUSIC", "Music"},         {"PICTURES", "Pictures"},  {"VIDEOS", "Videos"},
    };

    live = isLive();
    users = listUsers();
    i686 = isi686();

    QString distro_version_file;
    if (QFileInfo::exists("/etc/mx-version"))
        distro_version_file = "/etc/mx-version";
    else if (QFileInfo::exists("/etc/antix-version"))
        distro_version_file = "/etc/antix-version";

    if (QFileInfo::exists("/etc/lsb-release"))
        project_name = shell->getCmdOut(QStringLiteral("grep -oP '(?<=DISTRIB_ID=).*' /etc/lsb-release"));
    else
        project_name = shell->getCmdOut("lsb_release -i | cut -f2");
    project_name.replace(QLatin1String("\""), QLatin1String(""));
    if (!distro_version_file.isEmpty()) {
        distro_version = shell->getCmdOut("cut -f1 -d'_' " + distro_version_file);
        distro_version.remove(QRegularExpression("^" + project_name + "_|^" + project_name + "-"));
    } else {
        distro_version = shell->getCmdOut("lsb_release -r | cut -f2");
    }
    full_distro_name = project_name + "-" + distro_version + "_" + QString(i686 ? "386" : "x64");
    release_date = QDate::currentDate().toString(QStringLiteral("MMMM dd, yyyy"));
    if (QFileInfo::exists("/etc/lsb-release"))
        codename = shell->getCmdOut(QStringLiteral("grep -oP '(?<=DISTRIB_CODENAME=).*' /etc/lsb-release"));
    else
        codename = shell->getCmdOut("lsb_release -c | cut -f2");
    codename.replace(QLatin1String("\""), QLatin1String(""));
    boot_options = live ? readKernelOpts() : filterOptions(readKernelOpts());
}

// Create the output filename
QString Settings::getFilename() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (stamp == QLatin1String("datetime")) {
        return snapshot_basename + "-" + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmm"))
               + ".iso";
    } else {
        QString name;
        QDir dir;
        int n = 1;
        do {
            name = snapshot_basename + QString::number(n) + ".iso";
            dir.setPath("\"" + snapshot_dir + "/" + name + "\"");
            n++;
        } while (QFileInfo::exists(dir.absolutePath()));
        return name;
    }
}

quint64 Settings::getLiveRootSpace() const
{
    // rootspaceneeded is the size of the linuxfs file * a compression factor + contents of the rootfs.  conservative
    // but fast factors are same as used in live-remaster

    // load some live variables
    QSettings livesettings(QStringLiteral("/live/config/initrd.out"), QSettings::NativeFormat);
    QString sqfile_full = livesettings.value(QStringLiteral("SQFILE_FULL"), "/live/boot-dev/antiX/linuxfs").toString();

    // get compression factor by reading the linuxfs squasfs file, if available
    QString linuxfs_compression_type
        = shell->getCmdOut("dd if=" + sqfile_full + " bs=1 skip=20 count=2 status=none 2>/dev/null |od -An -tdI")
              .trimmed();
    const quint8 default_factor = 30;
    quint8 c_factor = default_factor;
    // gzip, xz, or lz4
    if (linuxfs_compression_type == QLatin1String("1"))
        c_factor = compression_factor.value(QStringLiteral("gzip"));
    else if (linuxfs_compression_type == QLatin1String("2"))
        c_factor = compression_factor.value(QStringLiteral("lzo")); // lzo, not used by antiX
    else if (linuxfs_compression_type == QLatin1String("3"))
        c_factor = compression_factor.value(QStringLiteral("lzma")); // lzma, not used by antiX
    else if (linuxfs_compression_type == QLatin1String("4"))
        c_factor = compression_factor.value(QStringLiteral("xz"));
    else if (linuxfs_compression_type == QLatin1String("5"))
        c_factor = compression_factor.value(QStringLiteral("lz4"));
    else if (linuxfs_compression_type == QLatin1String("6"))
        c_factor = compression_factor.value(QStringLiteral("zstd"));
    else
        c_factor = default_factor; // anything else or linuxfs not reachable (toram), should be pretty conservative

    quint64 rootfs_file_size = 0;
    quint64 linuxfs_file_size
        = shell->getCmdOut(QStringLiteral("df -k /live/linux --output=used --total |tail -n1")).toULongLong() * 100
          / c_factor;
    if (QFileInfo::exists(QStringLiteral("/live/persist-root")))
        rootfs_file_size = shell->getCmdOut(QStringLiteral("df -k /live/persist-root --output=used --total |tail -n1"))
                               .toULongLong();

    // add rootfs file size to the calculated linuxfs file size.  probaby conservative, as rootfs will likely have some
    // overlap with linuxfs
    return linuxfs_file_size + rootfs_file_size;
}

QString Settings::getUsedSpace()
{
    constexpr float factor = 1024 * 1024;
    QString out = "\n- " + QObject::tr("Used space on / (root): ");
    if (bool ok = false; live) {
        root_size = getLiveRootSpace();
        out += QString::number(root_size / factor, 'f', 2) + QStringLiteral("GiB -- ") + QObject::tr("estimated");
    } else {
        root_size = shell->getCmdOut(QStringLiteral("df -k --output=used / |tail -n1")).toULongLong(&ok);
        if (!ok)
            return QStringLiteral("Can't calculate free space on root");
        out += QString::number(root_size / factor, 'f', 2) + QStringLiteral("GiB");
    }
    if (bool ok = false; shell->run(QStringLiteral("mountpoint -q /home"))) {
        home_size = shell->getCmdOut(QStringLiteral("df -k --output=used /home |tail -n1")).toULongLong(&ok);
        if (!ok)
            return QStringLiteral("Can't calculate free space on /home");
        out.append("\n- " + QObject::tr("Used space on /home: ") + QString::number(home_size / factor, 'f', 2)
                   + QStringLiteral("GiB"));
    } else {
        home_size = 0; // /home on root
    }
    return out;
}

// Check if running from a 32bit environment
bool Settings::isi686() const { return (shell->getCmdOut(QStringLiteral("uname -m"), true) == QLatin1String("i686")); }

// Check if running from a live envoronment
bool Settings::isLive() const { return (shell->run(QStringLiteral("mountpoint -q /live/aufs"), true)); }

// checks if the directory is on a Linux partition
bool Settings::isOnSupportedPart(const QString &dir) const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Supported partition types (NTFS returns fuseblk)
    QStringList supported_partitions {QStringLiteral("ext2/ext3"), QStringLiteral("btrfs"), QStringLiteral("jfs"),
                                      QStringLiteral("reiserfs"),  QStringLiteral("xfs"),   QStringLiteral("fuseblk"),
                                      QStringLiteral("ramfs"),     QStringLiteral("tmpfs")};
    QString part_type = shell->getCmdOut("stat --file-system --format=%T \"" + dir + "\"").trimmed();
    qDebug() << "detected partition" << part_type << "supported part:" << supported_partitions.contains(part_type);
    return supported_partitions.contains(part_type);
}

// return the directory that has more free space available
QString Settings::largerFreeSpace(const QString &dir1, const QString &dir2) const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (shell->getCmdOut("stat -c '%d' " + dir1) == shell->getCmdOut("stat -c '%d' " + dir2))
        return dir1;
    quint64 dir1_free = getFreeSpace(dir1);
    quint64 dir2_free = getFreeSpace(dir2);
    return dir1_free >= dir2_free ? dir1 : dir2;
}

// return the directory that has more free space available
QString Settings::largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3) const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    return largerFreeSpace(largerFreeSpace(dir1, dir2), dir3);
}

QString Settings::getFreeSpaceStrings(const QString &path)
{
    constexpr float factor = 1024 * 1024;
    free_space = getFreeSpace(path);
    QString out = QString::number(free_space / factor, 'f', 2) + "GiB";

    qDebug().noquote() << QString("- " + QObject::tr("Free space on %1, where snapshot folder is placed: ").arg(path)
                                  + out)
                       << "\n";

    qDebug().noquote() << QObject::tr(
                              "The free space should be sufficient to hold the compressed data from / and /home\n\n"
                              "      If necessary, you can create more available space\n"
                              "      by removing previous snapshots and saved copies:\n"
                              "      %1 snapshots are taking up %2 of disk space.\n")
                              .arg(QString::number(getSnapshotCount()), getSnapshotSize());
    return out;
}

// return a list of users that have folders in /home
QStringList Settings::listUsers() const
{
    return shell->getCmdOut(QStringLiteral("lslogins --noheadings -u -o user |grep -vw root"), true)
        .split(QStringLiteral("\n"));
}

void Settings::excludeItem(const QString &item)
{
    if (item == QObject::tr("Desktop") || item == QLatin1String("Desktop"))
        excludeDesktop(true);
    if (item == QObject::tr("Documents") || item == QLatin1String("Documents"))
        excludeDocuments(true);
    if (item == QObject::tr("Downloads") || item == QLatin1String("Downloads"))
        excludeDownloads(true);
    if (item == QObject::tr("Music") || item == QLatin1String("Music"))
        excludeMusic(true);
    if (item == QObject::tr("Networks") || item == QLatin1String("Networks"))
        excludeNetworks(true);
    if (item == QObject::tr("Pictures") || item == QLatin1String("Pictures"))
        excludePictures(true);
    if (item == QLatin1String("Steam"))
        excludeSteam(true);
    if (item == QObject::tr("Videos") || item == QLatin1String("Videos"))
        excludeVideos(true);
    if (item == QLatin1String("VirtualBox"))
        excludeVirtualBox(true);
}

void Settings::excludeDesktop(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Desktop);
    QString exclusion = "/home/*/Desktop/!(minstall.desktop)" + getXdgUserDirs(QStringLiteral("DESKTOP"));
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeDocuments(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Documents);
    QString folder = QStringLiteral("home/*/Documents/");
    QString xdg_name = QStringLiteral("DOCUMENTS");
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeDownloads(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Downloads);
    QString folder = QStringLiteral("home/*/Downloads/");
    QString xdg_name = QStringLiteral("DOWNLOAD");
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeMusic(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Music);
    QString folder = QStringLiteral("home/*/Music/");
    QString xdg_name = QStringLiteral("MUSIC");
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeNetworks(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Networks);
    addRemoveExclusion(exclude, QStringLiteral("/etc/NetworkManager/system-connections/*"));
    addRemoveExclusion(exclude, QStringLiteral("/etc/wicd/*"));
    addRemoveExclusion(exclude, QStringLiteral("/var/lib/connman/*"));
}

void Settings::excludePictures(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Pictures);
    QString folder = QStringLiteral("home/*/Pictures/");
    QString xdg_name = QStringLiteral("PICTURES");
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeSteam(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Steam);
    addRemoveExclusion(exclude, QStringLiteral("home/*/.steam"));
    addRemoveExclusion(exclude, QStringLiteral("home/*/.local/share/Steam"));
}

void Settings::excludeSwapFile()
{
    QFile file("/etc/fstab");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open /etc/fstab";
        return;
    }

    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.startsWith("/") && !line.startsWith("/dev/")) {
            QStringList parts = line.split(QRegExp("\\s+"));
            if (parts.size() > 3) {
                if (parts.at(2) == "swap")
                    addRemoveExclusion(true, parts[0].remove(0, 1));
            }
        }
    }
}

void Settings::excludeVideos(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::Videos);
    QString folder = QStringLiteral("home/*/Videos/");
    QString xdg_name = QStringLiteral("VIDEOS");
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeVirtualBox(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude)
        exclusions.setFlag(Exclude::VirtualBox);
    addRemoveExclusion(exclude, QStringLiteral("home/*/VirtualBox VMs"));
}

// load settings from config file
void Settings::loadConfig()
{
    config_file.setFileName("/etc/" + qApp->applicationName() + ".conf");
    QSettings settings(config_file.fileName(), QSettings::IniFormat);

    session_excludes = QLatin1String("");
    snapshot_dir = settings.value(QStringLiteral("snapshot_dir"), QStringLiteral("/home/snapshot")).toString();
    if (!snapshot_dir.endsWith(QLatin1String("/snapshot")))
        snapshot_dir
            += (snapshot_dir.endsWith(QLatin1String("/")) ? QStringLiteral("snapshot") : QStringLiteral("/snapshot"));
    snapshot_excludes.setFileName(
        settings
            .value(QStringLiteral("snapshot_excludes"), QStringLiteral("/usr/local/share/excludes/")
                                                            + qApp->applicationName() + QStringLiteral("-exclude.list"))
            .toString());
    snapshot_basename = settings.value(QStringLiteral("snapshot_basename"), "snapshot").toString();
    make_md5sum = settings.value(QStringLiteral("make_md5sum"), "no").toString() != QLatin1String("no");
    make_sha512sum = settings.value(QStringLiteral("make_sha512sum"), "no").toString() != QLatin1String("no");
    make_isohybrid = settings.value(QStringLiteral("make_isohybrid"), "yes").toString() == QLatin1String("yes");
    compression = settings.value(QStringLiteral("compression"), "zstd").toString();
    mksq_opt = settings.value(QStringLiteral("mksq_opt")).toString();
    edit_boot_menu = settings.value(QStringLiteral("edit_boot_menu"), "no").toString() != QLatin1String("no");
    gui_editor = settings.value(QStringLiteral("gui_editor")).toString();
    stamp = settings.value(QStringLiteral("stamp")).toString();
    force_installer = settings.value(QStringLiteral("force_installer"), "true").toBool();
    tempdir_parent = settings.value(QStringLiteral("workdir")).toString();
    reset_accounts = false;
}

void Settings::excludeAll()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    excludeDesktop(true);
    excludeDocuments(true);
    excludeDownloads(true);
    excludeMusic(true);
    excludeNetworks(true);
    excludePictures(true);
    excludeSteam(true);
    excludeVideos(true);
    excludeVirtualBox(true);
}

void Settings::otherExclusions()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // add exclusions snapshot and work dirs
    addRemoveExclusion(true, snapshot_dir);
    addRemoveExclusion(true, work_dir);

    if (reset_accounts) {
        addRemoveExclusion(true, QStringLiteral("/etc/minstall.conf"));
        // exclude /etc/localtime if link and timezone not America/New_York
        if (shell->run(QStringLiteral("test -L /etc/localtime"))
            && shell->getCmdOut(QStringLiteral("cat /etc/timezone")) != QLatin1String("America/New_York"))
            addRemoveExclusion(true, QStringLiteral("/etc/localtime"));
    }
    excludeSwapFile();
}

void Settings::processArgs(const QCommandLineParser &arg_parser)
{
    shutdown = arg_parser.isSet(QStringLiteral("shutdown"));
    kernel = arg_parser.value(QStringLiteral("kernel"));
    preempt = arg_parser.isSet(QStringLiteral("preempt"));
    if (!arg_parser.value(QStringLiteral("directory")).isEmpty()
        && QFileInfo::exists(arg_parser.value(QStringLiteral("directory"))))
        snapshot_dir = arg_parser.value(QStringLiteral("directory"))
                       + (snapshot_dir.endsWith(QLatin1String("/")) ? "snapshot" : "/snapshot");

    if (!arg_parser.value(QStringLiteral("workdir")).isEmpty()
        && QFileInfo::exists(arg_parser.value(QStringLiteral("workdir"))))
        tempdir_parent = arg_parser.value(QStringLiteral("workdir"));

    if (!arg_parser.value(QStringLiteral("file")).isEmpty())
        snapshot_name
            = arg_parser.value(QStringLiteral("file"))
              + (arg_parser.value(QStringLiteral("file")).endsWith(QLatin1String(".iso")) ? QString()
                                                                                          : QStringLiteral(".iso"));
    else
        snapshot_name = getFilename();
    if (QFile::exists(snapshot_dir + "/" + snapshot_name)) {
        QString message
            = QObject::tr("Output file %1 already exists. Please use another file name, or delete the existent file.")
                  .arg(snapshot_dir + "/" + snapshot_name);
        if (qApp->metaObject()->className() != QLatin1String("QApplication"))
            qDebug().noquote() << message;
#ifndef CLI_BUILD
        else
            QMessageBox::critical(nullptr, QObject::tr("Error"), message);
#endif
        exit(EXIT_FAILURE);
    }
    reset_accounts = arg_parser.isSet(QStringLiteral("reset"));
    if (reset_accounts) {
        excludeAll();
    }
    if (arg_parser.isSet(QStringLiteral("month"))) {
        reset_accounts = true;
    }
    if (arg_parser.isSet(QStringLiteral("checksums"))) {
        make_sha512sum = true;
        make_md5sum = true;
    }
    if (arg_parser.isSet(QStringLiteral("month"))) {
        make_sha512sum = true;
        make_md5sum = false;
    }
    if (arg_parser.isSet(QStringLiteral("no-checksums"))) {
        make_sha512sum = false;
        make_md5sum = false;
    }
    if (!arg_parser.value(QStringLiteral("compression")).isEmpty()) {
        compression = arg_parser.value(QStringLiteral("compression"));
    }
    if (!arg_parser.value(QStringLiteral("compression-level")).isEmpty()) {
        mksq_opt = arg_parser.value(QStringLiteral("compression-level"));
    }
    selectKernel();
}

void Settings::processExclArgs(const QCommandLineParser &arg_parser)
{
    if (!arg_parser.values(QStringLiteral("exclude")).isEmpty()) {
        QStringList options = arg_parser.values(QStringLiteral("exclude"));
        QStringList valid_options {"Desktop",  "Documents", "Downloads", "Music",     "Networks",
                                   "Pictures", "Steam",     "Videos",    "VirtualBox"};
        for (const QString &option : options)
            if (valid_options.contains(option))
                excludeItem(option);
    }
}

// Read kernel line and options from /proc/cmdline
QString Settings::readKernelOpts() const
{
    QFile file(QStringLiteral("/proc/cmdline"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file:" << file.fileName();
        return QString();
    }
    QString proc_cmdline = file.readAll().trimmed();
    QString conf_cmdline
        = shell->getCmdOut("sed -nr 's/^CONFIG_CMDLINE=\"(.*)\"$/\\1/p' /boot/config-$(uname -r) 2>/dev/null");
    QString krnl_cmdline = proc_cmdline.replace(conf_cmdline, "").trimmed();
    krnl_cmdline.remove(QRegularExpression("^BOOT_IMAGE=\\S* ?"));
    return krnl_cmdline.trimmed();
}

QString Settings::filterOptions(QString options)
{
    options.remove(QRegularExpression(R"(\b(initrd|init|root|resume|resume_offset|cryptsetup)=\S*\s?)"));
    options.remove(QRegularExpression(R"(\bro\s?\b)"));
    return options.trimmed();
}

void Settings::setMonthlySnapshot(const QCommandLineParser &arg_parser)
{
    QString name;
    if (QFileInfo::exists(QStringLiteral("/etc/mx-version"))) {
        name = shell->getCmdOut(QStringLiteral("cat /etc/mx-version |cut -f1 -d' '"));
    } else {
        qDebug() << "/etc/mx-version not found. Not MX Linux?";
        name = "MX_" + QString(i686 ? "386" : "x64");
    }
    if (arg_parser.value("file").isEmpty()) {
        auto month = QDate::currentDate().toString("MMMM");
        auto suffix = name.section("_", 1, 1);
        if (qgetenv("DESKTOP_SESSION") == "plasma")
            suffix = "KDE";
        snapshot_name = name.section("_", 0, 0) + "_" + month + "_" + suffix + ".iso";
    }
    if (QFile::exists(snapshot_dir + "/" + snapshot_name)) {
        QString message
            = QObject::tr("Output file %1 already exists. Please use another file name, or delete the existent file.")
                  .arg(snapshot_dir + "/" + snapshot_name);
        if (qApp->metaObject()->className() != QLatin1String("QApplication"))
            qDebug().noquote() << message;
#ifndef CLI_BUILD
        else
            QMessageBox::critical(nullptr, QObject::tr("Error"), message);
#endif
        exit(EXIT_FAILURE);
    }
    if (arg_parser.value(QStringLiteral("compression")).isEmpty())
        compression = QStringLiteral("zstd");
    reset_accounts = true;
    boot_options.remove("toram");
    excludeAll();
}
