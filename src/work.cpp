/**********************************************************************
 *  work.cpp
 **********************************************************************
 * Copyright (C) 2020-2025 MX Authors
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

#include "work.h"

#include <QDate>
#include <QDebug>
#include <QDirIterator>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QTextStream>

#include <stdexcept>

#include "filesystemutils.h"

Work::Work(Settings *settings, QObject *parent)
    : QObject(parent),
      settings(settings)
{
    if (!settings) {
        qCritical() << "Work constructor: Settings pointer cannot be null";
        throw std::invalid_argument("Settings pointer cannot be null");
    }

    qDebug() << "Work object initialized for settings:" << settings->snapshotName;
}

bool Work::isEnvironmentReady() const
{
    // Check if work directory exists and is accessible
    if (settings->workDir.isEmpty() || !QDir(settings->workDir).exists()) {
        return false;
    }

    // Check if snapshot directory is accessible
    if (settings->snapshotDir.isEmpty() || !QDir(settings->snapshotDir).exists()) {
        return false;
    }

    // Check if required tools are available
    const QStringList requiredTools {"mksquashfs", "xorriso"};
    for (const QString &tool : requiredTools) {
        if (QStandardPaths::findExecutable(tool).isEmpty()) {
            return false;
        }
    }

    return true;
}

// Checks if there's enough space on partitions, if not post error, cleanup and exit
// We don't yet take /home used space into considerations (need to calculate how much is excluded)
void Work::checkEnoughSpace()
{
    quint64 required_space = getRequiredSpace();
    // Check foremost if enough space for ISO on snapshot_dir, print error and exit if not
    checkNoSpaceAndExit(required_space, settings->freeSpace, settings->snapshotDir);

    /* If snapshot and workdir are on the same partition we need about double the size of required_space.
     * If both TMP work_dir and ISO don't fit on snapshot_dir, see if work_dir can be put on /home or /tmp
     * we already checked that ISO can fit on snapshot_dir so if TMP work fits on /home or /tmp move
     * the work_dir to the appropriate place and return */
    if (QStorageInfo(settings->workDir + "/").device() == QStorageInfo(settings->snapshotDir + "/").device()) {
        if (settings->freeSpace < required_space * 2) {
            if (checkAndMoveWorkDir("/tmp", required_space)) {
                return;
            }
            if (checkAndMoveWorkDir("/home", required_space)) {
                return;
            }
            checkNoSpaceAndExit(required_space * 2, settings->freeSpace,
                                settings->snapshotDir); // Print out error and exit
        }
    } else { // If not on the same partitions, check if work_dir has enough free space for temp files for required_space
        checkNoSpaceAndExit(required_space, settings->freeSpaceWork, settings->workDir);
    }
}

bool Work::checkInstalled(const QString &package)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    // Validate package name contains only safe characters to prevent command injection
    // Debian package names allow: lowercase letters, digits, plus, minus, dot.
    // Also allow optional architecture qualifiers like ":amd64".
    static const QRegularExpression validPackageName("^[a-z0-9+.:-]+$");
    if (!validPackageName.match(package).hasMatch()) {
        qWarning() << "Invalid package name:" << package;
        return false;
    }

    QProcess dpkgQuery;
    dpkgQuery.start("dpkg-query", {"-W", "-f=${Status}", "--", package});
    if (!dpkgQuery.waitForFinished(5000)) {
        dpkgQuery.kill();
        dpkgQuery.waitForFinished(1000);
        return false;
    }
    if (dpkgQuery.exitStatus() != QProcess::NormalExit || dpkgQuery.exitCode() != 0) {
        return false;
    }
    const QByteArray status = dpkgQuery.readAllStandardOutput();
    return status.contains("install ok installed");
}

void Work::cleanUp()
{
    const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
    Cmd().runAsRoot(snapshotLib + " chown_conf", Cmd::QuietMode::Yes);
    if (!started) {
        shell.close();
        initrd_dir.remove();
        exit(EXIT_SUCCESS);
    }
    emit message(tr("Cleaning..."));
    QTextStream out(stdout);
    out << "\033[?25h";
    out.flush();
    Cmd().runAsRoot(snapshotLib + " kill_mksquashfs", Cmd::QuietMode::Yes);
    shell.close();
    QProcess::execute("sync", {});
    QDir::setCurrent("/");
    if (QFileInfo::exists("/tmp/installed-to-live/cleanup.conf")) {
        Cmd().runAsRoot(snapshotLib + " cleanup");
    }
    initrd_dir.remove();
    settings->tmpdir.reset();
    if (done) {
        emit message(tr("Done"));
        Cmd().runAsRoot(snapshotLib + " copy_log", Cmd::QuietMode::Yes);
        if (settings->shutdown) {
            QFile::copy("/tmp/" + QCoreApplication::applicationName() + ".log",
                        settings->snapshotDir + "/" + settings->snapshotName + ".log");
            QProcess::execute("sync", {});
            QProcess::execute("dbus-send",
                              {"--system", "--print-reply", "--dest=org.freedesktop.login1", "/org/freedesktop/login1",
                               "org.freedesktop.login1.Manager.PowerOff", "boolean:true"});
        }
        exit(EXIT_SUCCESS);
    } else {
        emit message(tr("Interrupted or failed to complete"));
        Cmd().runAsRoot(snapshotLib + " copy_log", Cmd::QuietMode::Yes);
        exit(EXIT_FAILURE);
    }
}

// Check if we can put work_dir on another partition with enough space, move work_dir there and setupEnv again
bool Work::checkAndMoveWorkDir(const QString &dir, quint64 req_size)
{
    // See first if the dir is on different partition otherwise it's irrelevant
    if (QStorageInfo(dir + "/").device() != QStorageInfo(settings->snapshotDir + "/").device()
        && FileSystemUtils::getFreeSpace(dir) > req_size) {
        if (QFileInfo::exists("/tmp/installed-to-live/cleanup.conf")) {
            const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
            Cmd().runAsRoot(snapshotLib + " cleanup");
        }
        settings->tempDirParent = dir;
        if (!settings->checkTempDir()) {
            cleanUp();
        }
        setupEnv();
        return true;
    }
    return false;
}

void Work::checkNoSpaceAndExit(quint64 needed_space, quint64 free_space, const QString &dir)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    constexpr float factor = 1024 * 1024;
    qDebug() << "Needed space:" << needed_space;
    qDebug() << "Free space  :" << free_space << "on" << dir;
    if (needed_space > free_space) {
        emit messageBox(
            BoxType::critical, tr("Error"),
            tr("There's not enough free space on your target disk, you need at least %1")
                    .arg(QString::number(static_cast<double>(needed_space) / factor, 'f', 2) + "GiB")
                + "\n"
                + tr("You have %1 free space on %2")
                      .arg(QString::number(static_cast<double>(free_space) / factor, 'f', 2) + "GiB", dir)
                + "\n"
                + tr("If you are sure you have enough free space rerun the program with -o/--override-size option"));
        cleanUp();
    }
}

void Work::closeInitrd(const QString &initrd_dir, const QString &file)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QDir::setCurrent(initrd_dir);
    shell.run("(find . |cpio -o -H newc --owner root:root |gzip -9) >\"" + file + "\"");
    makeChecksum(HashType::md5, settings->workDir + "/iso-template/antiX", "initrd.gz");
}

// copyModules(mod_dir/kernel kernel)
void Work::copyModules(const QString &to, const QString &kernel)
{
    shell.run(QString(R"(/usr/share/%1/scripts/copy-initrd-modules -e -t="%2" -k="%3")")
                  .arg(qApp->applicationName(), to, kernel));
    shell.runAsRoot(QString("/usr/share/%1/scripts/copy-initrd-programs -e --to=\"%2\"").arg(qApp->applicationName(), to));
    shell.runAsRoot("chown -R $(logname): " + to);
}

// Copying the iso-template filesystem
void Work::copyNewIso()
{
    emit message(tr("Copying the new-iso filesystem..."));
    QDir::setCurrent(settings->workDir);

    shell.run("tar xf /usr/lib/iso-template/iso-template.tar.gz");
    shell.run("cp /usr/lib/iso-template/template-initrd.gz iso-template/antiX/initrd.gz");
    shell.run("cp /boot/vmlinuz-" + settings->kernel + " iso-template/antiX/vmlinuz");

    replaceMenuStrings();
    makeChecksum(HashType::md5, settings->workDir + "/iso-template/antiX", "vmlinuz");

    QString path = initrd_dir.path();
    if (!initrd_dir.isValid()) {
        qDebug() << tr("Could not create temp directory. ") + path;
        cleanUp();
    }

    openInitrd(settings->workDir + "/iso-template/antiX/initrd.gz", path);

    // Strip modules; make sure initrd_dir is correct to avoid disaster
    if (path.startsWith("/tmp/")) {
        QDir modulesDir(path + "/lib/modules");
        if (modulesDir.exists()) {
            modulesDir.removeRecursively();
        }
    }

    // For old versions we copy initrd-release for newer ones we copy initrd_release
    QString sourcePath = "/etc/initrd-release";
    QString destinationPath = path + "/etc/initrd-release";
    QFileInfo sourceFileInfo(sourcePath);
    if (sourceFileInfo.exists() && sourceFileInfo.isFile()) {
        if (!QFile::exists(destinationPath) || QFile::remove(destinationPath)) {
            QFile::copy(sourcePath, destinationPath);
        }
    }
    sourcePath = "/etc/initrd_release";
    destinationPath = path + "/etc/initrd_release";
    sourceFileInfo.setFile(sourcePath);
    if (sourceFileInfo.exists() && sourceFileInfo.isFile()) {
        if (!QFile::exists(destinationPath) || QFile::remove(destinationPath)) {
            QFile::copy(sourcePath, destinationPath);
        }
    }

    if (initrd_dir.isValid()) {
        copyModules(path, settings->kernel);
        closeInitrd(path, settings->workDir + "/iso-template/antiX/initrd.gz");
        initrd_dir.remove();
    }
}

// Create squashfs and then the iso
bool Work::createIso(const QString &filename)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Squash the filesystem copy
    QString unbuffer = checkInstalled("expect") ? "unbuffer " : "stdbuf -o0 ";
    using Release::Version;
    QString throttle
        = (Settings::getDebianVerNum() < Version::Bookworm) ? "" : " -throttle " + QString::number(settings->throttle);
    QString cmd = unbuffer + "mksquashfs /.bind-root \"" + settings->workDir + "/iso-template/antiX/linuxfs\" -comp "
                  + settings->compression + " -processors " + QString::number(settings->cores) + throttle
                  + (settings->mksqOpt.isEmpty() ? "" : " " + settings->mksqOpt) + " -wildcards -ef "
                  + settings->snapshotExcludes.fileName()
                  + (settings->sessionExcludes.isEmpty() ? "" : " -e " + settings->sessionExcludes);
    if (Cmd().getOut("umask", Cmd::QuietMode::Yes) != "0022") {
        cmd.prepend("umask 022; ");
    }
    emit message(tr("Squashing filesystem..."));
    if (!shell.runAsRoot(cmd)) {
        emit messageBox(
            BoxType::critical, tr("Error"),
            tr("Could not create linuxfs file, please check /var/log/%1.log").arg(QCoreApplication::applicationName()));
        return false;
    }
    writeUnsquashfsSize(shell.readAllOutput());

    // Move linuxfs files to iso-2/antiX folder
    QDir().mkpath("iso-2/antiX");
    shell.run("mv iso-template/antiX/linuxfs* iso-2/antiX");
    makeChecksum(HashType::md5, settings->workDir + "/iso-2/antiX", "linuxfs");

    const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
    Cmd().runAsRoot(snapshotLib + " cleanup");

    // Create the iso file
    QDir::setCurrent(settings->workDir + "/iso-template");
    cmd = "xorriso -as mkisofs -l -V MXLIVE -R -J -pad -iso-level 3 -no-emul-boot -boot-load-size 4 -boot-info-table "
          "-b boot/isolinux/isolinux.bin -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -c "
          "boot/isolinux/isolinux.cat -o \""
          + settings->snapshotDir + "/" + filename + "\" . \"" + settings->workDir + "/iso-2\"";
    emit message(tr("Creating CD/DVD image file..."));
    if (!shell.run(cmd)) {
        emit messageBox(
            BoxType::critical, tr("Error"),
            tr("Could not create ISO file, please check whether you have enough space on the destination partition."));
        return false;
    }

    // Make it isohybrid
    if (settings->makeIsohybrid) {
        emit message(tr("Making hybrid iso"));
        shell.run("isohybrid --uefi \"" + settings->snapshotDir + "/" + filename + "\"");
    }

    // Make ISO checksums
    if (settings->makeMd5sum) {
        makeChecksum(HashType::md5, settings->snapshotDir, filename);
    }
    if (settings->makeSha512sum) {
        makeChecksum(HashType::sha512, settings->snapshotDir, filename);
    }

    auto elapsedTime = QTime(0, 0).addMSecs(e_timer.elapsed());
    emit message(tr("Done"));
    if (settings->shutdown) {
        done = true;
        cleanUp();
    }
    emit messageBox(BoxType::information, tr("Success"),
                    tr("MX Snapshot completed successfully!") + '\n'
                        + tr("Snapshot took %1 to finish.").arg(elapsedTime.toString("hh:mm:ss")) + "\n\n"
                        + tr("Thanks for using MX Snapshot, run MX Live USB Maker next!"));
    done = true;
    return true;
}

bool Work::installPackage(const QString &package)
{
    emit message(tr("Installing ") + package);
    shell.runAsRoot("apt-get update");
    if (!shell.runAsRoot("apt-get install -y " + package)) {
        emit messageBox(BoxType::critical, tr("Error"), tr("Could not install ") + package);
        return false;
    }
    return true;
}

// Create checksums for different files
void Work::makeChecksum(Work::HashType hash_type, const QString &folder, const QString &file_name)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    emit message(tr("Calculating checksum..."));
    shell.run("sync");
    QDir::setCurrent(folder);
    QString ce = QVariant::fromValue(hash_type).toString();
    QString cmd;
    QString checksum_cmd = QString("%1sum \"%2\">\"%3/%2.%1\"").arg(ce, file_name, folder);
    QString temp_dir {"/tmp/snapshot-checksum-temp"};
    QString checksum_tmp
        = QString(
              "TD=%1; KEEP=$TD/.keep; [ -d $TD ] || mkdir $TD ; FN=\"%2\"; CF=\"%3/${FN}.%4\"; cp $FN $TD/$FN; pushd "
              "$TD>/dev/null; %4sum $FN > $FN.%4 ; cp $FN.%4 $CF; popd >/dev/null ; [ -e $KEEP ] || rm -rf $TD")
              .arg(temp_dir, file_name, folder, ce);

    if (settings->preempt) {
        // Check free space available on /tmp
        shell.run(QString("TF=%1/\"%2\"; [ -f \"$TF\" ] && rm -f \"$TF\"").arg(temp_dir, file_name));
        if (!shell.run(QString("DUF=$(du -BM \"%1\" |grep -oE '^[[:digit:]]+'); TDA=$(df -BM --output=avail /tmp |grep -oE "
                               "'^[[:digit:]]+'); ((TDA/10*8 >= DUF))")
                           .arg(file_name))) {
            settings->preempt = false;
        }
    }
    if (!settings->preempt) {
        cmd = checksum_cmd;
    } else {
        // Free pagecache
        shell.run("sync; sleep 1");
        const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
        Cmd().runAsRoot(snapshotLib + " drop_caches");
        shell.run("sleep 1");
        cmd = checksum_tmp;
    }
    shell.run(cmd);
    QDir::setCurrent(settings->workDir);
}

void Work::openInitrd(const QString &file, const QString &initrd_dir)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    emit message(tr("Building new initrd..."));
    shell.run("chmod a+rx \"" + initrd_dir + "\"");
    QDir::setCurrent(initrd_dir);
    shell.run(QString("gunzip -c \"%1\" |cpio -idum").arg(file));
}

// Replace text in menu items in grub.cfg, syslinux.cfg, isolinux.cfg
void Work::replaceMenuStrings()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QString fullDistroNameSpace = settings->fullDistroName;
    fullDistroNameSpace.replace("_", " ");

    const QString grub_cfg {"/iso-template/boot/grub/grub.cfg"};
    replaceStringInFile("%DISTRO%", settings->projectName + "-" + settings->distroVersion,
                        settings->workDir + grub_cfg);
    replaceStringInFile("%DISTRO_NAME%", settings->projectName, settings->workDir + grub_cfg);
    replaceStringInFile("%FULL_DISTRO_NAME%", settings->fullDistroName, settings->workDir + grub_cfg);
    replaceStringInFile("%FULL_DISTRO_NAME_SPACE%", fullDistroNameSpace, settings->workDir + grub_cfg);
    replaceStringInFile("%RELEASE_DATE%", settings->releaseDate, settings->workDir + grub_cfg);

    const QString grubenv_cfg {"/iso-template/boot/grub/grubenv.cfg"};
    const QString boot_pararameter_regexp {"(lang|kbd|kbvar|kbopt|tz)=[^[:space:]]*"};
    shell.run(QString("printf '%s\\n' %1 | grep -E '^%2' >> '%3'")
                  .arg(settings->bootOptions, boot_pararameter_regexp, settings->workDir + grubenv_cfg));
    shell.run(
        QString(
            R"(sed -i "s|%OPTIONS%|$(sed -r 's/[[:space:]]%2/ /g; s/^[[:space:]]+//; s/[[:space:]]+/ /g'<<<' %1')|" '%3')")
            .arg(settings->bootOptions, boot_pararameter_regexp, settings->workDir + grub_cfg));
    const QString syslinux_cfg {"/iso-template/boot/syslinux/syslinux.cfg"};
    const QString isolinux_cfg {"/iso-template/boot/isolinux/isolinux.cfg"};
    for (const QString &file : {syslinux_cfg, isolinux_cfg}) {
        replaceStringInFile("%OPTIONS%", settings->bootOptions, settings->workDir + file);
        replaceStringInFile("%CODE_NAME%", settings->codename, settings->workDir + file);
    }

    const QString sys_readme = "/iso-template/boot/syslinux/readme.msg";
    const QString iso_readme = "/iso-template/boot/isolinux/readme.msg";
    const QStringList cfg_files {syslinux_cfg, isolinux_cfg, sys_readme, iso_readme};
    for (const QString &file : cfg_files) {
        replaceStringInFile("%FULL_DISTRO_NAME%", settings->fullDistroName, settings->workDir + file);
        replaceStringInFile("%RELEASE_DATE%", settings->releaseDate, settings->workDir + file);
    }

    QDir themeDir(settings->workDir + "/iso-template/boot/grub/theme");
    for (const QFileInfo &themeFile : themeDir.entryInfoList({"*.txt"}, QDir::Files)) {
        replaceStringInFile("%ASCII_CODE_NAME%", settings->codename, themeFile.absoluteFilePath());
        replaceStringInFile("%DISTRO%", settings->projectName + "-" + settings->distroVersion,
                            themeFile.absoluteFilePath());
    }
}

// Util function for replacing strings in files
bool Work::replaceStringInFile(const QString &old_text, const QString &new_text, const QString &file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file_path;
        return false;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();

    if (content.contains(old_text)) {
        content.replace(old_text, new_text);

        file.resize(0);
        stream.seek(0);
        stream << content;
        stream.flush();

        if (stream.status() != QTextStream::Ok) {
            qWarning() << "Failed to write to file:" << file_path;
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

// Save package list in working directory
void Work::savePackageList(const QString &file_name)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QFileInfo fi(file_name);
    QDir dir(settings->workDir + "/iso-template/" + fi.completeBaseName());
    if (!dir.mkpath(dir.absolutePath())) {
        emit messageBox(BoxType::critical, tr("Error"),
                        tr("Could not create working directory. ") + dir.absolutePath());
    }
    const QString fullName = QString("%1/iso-template/%2/package_list").arg(settings->workDir, fi.completeBaseName());
    const QString cmd = QString(R"(dpkg -l | awk '/^ii /{printf "%-41s %s\n", $2, $3}' > '%1')").arg(fullName);
    shell.run(cmd);
}

// Setup the environment before taking the snapshot
void Work::setupEnv()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Checks if work_dir looks OK
    if (!settings->workDir.contains("/mx-snapshot")) {
        cleanUp();
    }

    QString bind_boot;
    QString bind_boot_too;
    if (shell.run("mountpoint /boot")) {
        bind_boot = "bind=/boot ";
        bind_boot_too = ",/boot";
    }

    // Install mx-installer if absent
    if (settings->forceInstaller && !checkInstalled("mx-installer")) {
        installPackage("mx-installer");
    }

    writeSnapshotInfo();
    writeVersionFile();
    writeLsbRelease();

    // Setup environment if creating a respin (reset root/demo, remove personal accounts)
    if (settings->resetAccounts) {
        shell.runAsRoot("installed-to-live -b /.bind-root start " + bind_boot
                        + "empty=/home general version-file read-only");
    } else {
        shell.runAsRoot("installed-to-live -b /.bind-root start bind=/home" + bind_boot_too
                        + " live-files version-file adjtime read-only");
    }
}

void Work::writeLsbRelease()
{
    QString filePath = "/usr/local/share/live-files/files/etc/lsb-release";
    if (!QFile::exists(filePath)) {
        filePath = "/usr/share/live-files/files/etc/lsb-release";
    }
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream stream(&file);
    stream << "PRETTY_NAME=\"" << settings->projectName << " " << settings->distroVersion << " " << settings->codename
           << "\"\n";
    stream << "DISTRIB_ID=\"" << settings->projectName << "\"\n";
    stream << "DISTRIB_RELEASE=" << settings->distroVersion << "\n";
    stream << "DISTRIB_CODENAME=\"" << settings->codename << "\"\n";
    stream << "DISTRIB_DESCRIPTION=\"" << settings->projectName << " " << settings->distroVersion << " "
           << settings->codename << "\"\n";
    file.close();
}

// Write date of the snapshot in a "snapshot_created" file
void Work::writeSnapshotInfo()
{
    const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
    Cmd().runAsRoot(snapshotLib + " datetime_log", Cmd::QuietMode::Yes);
}

void Work::writeVersionFile()
{
    QString filePath = "/usr/local/share/live-files/files/etc/mx-version";
    if (!QFile::exists(filePath)) {
        filePath = "/usr/share/live-files/files/etc/mx-version";
    }
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream stream(&file);
    stream << settings->fullDistroName << " " << settings->codename << " " << settings->releaseDate << "\n";
    file.close();
}

void Work::writeUnsquashfsSize(const QString &text)
{
    QSettings file(settings->workDir + "/iso-template/antiX/linuxfs.info", QSettings::NativeFormat);
    file.setValue("UncompressedSizeKB",
                  text.section(QRegularExpression(" uncompressed filesystem size \\("), 1, 1).section(" ", 0, 0));
}

quint64 Work::getRequiredSpace()
{
    QStringList excludes;
    QFile *file = &settings->snapshotExcludes;

    // Open and read the excludes file
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open file: " << file->fileName();
    }
    while (!file->atEnd()) {
        QString line = file->readLine().trimmed();
        if (!line.startsWith('#') && !line.isEmpty() && !line.startsWith(".bind-root")) {
            excludes << line;
        }
    }
    file->close();

    // Add session excludes
    if (!settings->sessionExcludes.isEmpty()) {
        QString sessionExcludes = settings->sessionExcludes;
        QStringList excludeList = sessionExcludes.split("\" \"");
        excludes.reserve(excludeList.size());
        for (QString exclude : excludeList) {
            exclude = exclude.replace('"', "").trimmed();
            excludes << exclude;
        }
    }
    QString root_dev = Cmd().getOut("df /.bind-root --output=target |tail -1", Cmd::QuietMode::Yes);
    QMutableStringListIterator it(excludes);
    while (it.hasNext()) {
        it.next();
        if (it.value().indexOf('!') != -1) { // Truncate things like "!(minstall.desktop)"
            it.value().truncate(it.value().indexOf('!'));
        }
        it.value().replace(' ', "\\ "); // Escape special bash characters, might need to expand this
        it.value().replace('(', "\\(");
        it.value().replace(')', "\\)");
        it.value().replace('|', "\\|");
        it.value().prepend("/.bind-root/"); // Check size occupied by excluded files on /.bind-root only
        it.value().replace(QRegularExpression("/\\*$"), "/"); // Remove last *
        //  Remove from list if files not on the same volume
        if (root_dev != Cmd().getOut("df " + it.value() + " --output=target 2>/dev/null |tail -1", Cmd::QuietMode::Yes)) {
            it.remove();
        }
    }
    emit message(tr("Calculating total size of excluded files..."));
    bool ok = false;
    QString cmd = settings->live ? "du -sc" : "du -sxc";
    quint64 excl_size
        = shell.getOutAsRoot(cmd + " {" + excludes.join(',') + "} 2>/dev/null |tail -1 |cut -f1").toULongLong(&ok);
    if (!ok) {
        qDebug() << "Error: calculating size of excluded files\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    emit message(tr("Calculating size of root..."));
    cmd = settings->live ? "du -s" : "du -sx";
    quint64 root_size = shell.getOutAsRoot(cmd + " /.bind-root 2>/dev/null |tail -1 |cut -f1").toULongLong(&ok);
    qDebug() << "SIZE" << root_size;
    if (!ok) {
        qDebug() << "Error: calculating root size (/.bind-root)\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    qDebug() << "SIZE ROOT    " << root_size;
    qDebug() << "SIZE EXCLUDES" << excl_size;
    const uint c_factor = settings->compressionFactor.value(settings->compression);
    qDebug() << "COMPRESSION  " << c_factor;
    qDebug() << "SIZE NEEDED  " << (root_size - excl_size) * c_factor / 100;
    qDebug() << "SIZE FREE    " << settings->freeSpace << '\n';

    if (excl_size > root_size) {
        qDebug() << "Error: calculating excluded file size.\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    return (root_size - excl_size) * c_factor / 100;
}
