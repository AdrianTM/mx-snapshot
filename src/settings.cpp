/**********************************************************************
 *  settings.cpp
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

#include "settings.h"

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>

#include <exception>
#include <unistd.h>

#include "bindrootmanager.h"
#include "messagehandler.h"

namespace
{
QString trimQuotesValue(QString value)
{
    value = value.trimmed();
    if (value.startsWith('\'') && value.endsWith('\'') && value.length() > 1) {
        value = value.mid(1, value.length() - 2);
    }
    if (value.startsWith('"') && value.endsWith('"') && value.length() > 1) {
        value = value.mid(1, value.length() - 2);
    }
    return value;
}

QString readOsReleaseValue(const QString &key)
{
    QFile file("/etc/os-release");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream in(&file);
    const QString keyPrefix = key + "=";
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#') || !line.startsWith(keyPrefix)) {
            continue;
        }
        return trimQuotesValue(line.section('=', 1));
    }
    return {};
}

QString loggedInUserName()
{
    QString username = qEnvironmentVariable("SUDO_USER");
    if (username.isEmpty()) {
        username = qEnvironmentVariable("LOGNAME");
    }
    if (username.isEmpty()) {
        username = qEnvironmentVariable("USER");
    }

    if (username.isEmpty() || username == QLatin1String("root")) {
        const QString logname = Cmd().getOut("logname", Cmd::QuietMode::Yes).trimmed();
        if (!logname.isEmpty() && logname != QLatin1String("root")) {
            username = logname;
        }
    }

    return username;
}

QString userConfigBaseDir()
{
    const QString username = loggedInUserName();

    if (!username.isEmpty()) {
        const QString candidateHome = QDir::cleanPath("/home/" + username);
        if (QDir(candidateHome).exists()) {
            return QDir::cleanPath(candidateHome + "/.config");
        }
    }

    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

void chownFileToLoggedInUser(const QString &path)
{
    const QString username = loggedInUserName();
    if (username.isEmpty() || path.isEmpty()) {
        return;
    }
    const QFileInfo fileInfo(path);
    if (!fileInfo.exists() || fileInfo.isWritable()) {
        return;
    }
    const QString chownCmd = QStringLiteral("chown %1: \"%2\"").arg(username, path);
    Cmd().runAsRoot(chownCmd, Cmd::QuietMode::Yes);
}

} // namespace

Settings::Settings(const QCommandLineParser &argParser)
    : x86(SystemInfo::is386()),
      maxCores(Cmd().getOut("nproc", Cmd::QuietMode::Yes).trimmed().toUInt()),
      monthly(argParser.isSet("month")),
      overrideSize(argParser.isSet("override-size")),
      editBootMenu(getEditBootMenuSetting()),
      forceInstaller(getInitialSettings().forceInstaller),
      live(getInitialSettings().live),
      makeIsohybrid(getInitialSettings().makeIsohybrid),
      configFile("/etc/" + qApp->applicationName() + ".conf"),
      guiEditor(getInitialSettings().guiEditor),
      snapshotBasename(getInitialSettings().snapshotBasename),
      stamp(getInitialSettings().stamp),
      users(getInitialSettings().users)
{
    try {
        if (!initializeConfiguration()) {
            handleInitializationError(QObject::tr("Failed to initialize configuration"));
            exit(EXIT_FAILURE);
        }

        const QString appName = QCoreApplication::applicationName();
        const QString overlayBase = "/run/" + appName + "/bind-root-overlay";
        bool cleanupRan = false;
        bool cleanupOk = true;
        const QString elevateTool = Cmd::elevationTool();
        if (BindRootManager::hasCleanupState() || QFileInfo::exists(overlayBase)) {
            cleanupRan = true;
            Cmd shell;
            BindRootManager bindManager(shell, "/.bind-root", "/tmp/" + appName + "-bind-root-work");
            cleanupOk = bindManager.cleanup();
        }
        const QString overlayRoot = "/run/" + appName + "/bind-root-overlay/root";
        const bool bindRootMounted = Cmd().run("mountpoint -q /.bind-root", Cmd::QuietMode::Yes)
            || Cmd().run("mountpoint -q \"" + overlayRoot + "\"", Cmd::QuietMode::Yes);
        if (!cleanupRan || cleanupOk || !bindRootMounted) {
            Cmd().run(elevateTool + " /usr/lib/" + appName + "/snapshot-lib cleanup_overlay " + appName);
        }

        loadConfig(); // Load settings from .conf file
        setVariables();
        kernel = getInitialKernel(argParser); // Initialize kernel after config is loaded
        preempt = argParser.isSet("preempt"); // Initialize preempt from command line
        processArgs(argParser);

        if (monthly) {
            setMonthlySnapshot(argParser);
        }

        processExclArgs(argParser);

        // Validate final configuration
        if (!checkConfiguration()) {
            handleInitializationError(QObject::tr("Configuration validation failed"));
            exit(EXIT_FAILURE);
        }

    } catch (const std::exception &e) {
        handleInitializationError(QObject::tr("Exception during initialization: %1").arg(e.what()));
        exit(EXIT_FAILURE);
    } catch (...) {
        handleInitializationError(QObject::tr("Unknown exception during initialization"));
        exit(EXIT_FAILURE);
    }
}

// Check if compression is available in the kernel (lz4, lzo, xz)
bool Settings::checkCompression() const
{
    if (isArch) {
        return true;
    }
    if (compression == "gzip"
        || !QFileInfo::exists("/boot/config-"
                              + kernel)) { // Don't check for gzip or if the kernel config file is missing
        return true;
    }
    return Cmd().run("grep ^CONFIG_SQUASHFS_" + compression.toUpper() + "=y /boot/config-" + kernel);
}

// Adds or removes exclusion to the exclusion string
void Settings::addRemoveExclusion(bool add, QString exclusion)
{
    exclusion.remove(QRegularExpression("^/"));
    if (add) {
        sessionExcludes.append('"' + exclusion + "\" ");
    } else {
        sessionExcludes.remove('"' + exclusion + "\" ");
    }
}

bool Settings::checkSnapshotDir() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (!Cmd().runAsRoot("mkdir -p \"" + snapshotDir + '"', Cmd::QuietMode::No)) {
        qDebug() << QObject::tr("Could not create work directory. ") + snapshotDir;
        return false;
    }
    Cmd().runAsRoot("chown $(logname): \"" + snapshotDir + '"');
    return true;
}

bool Settings::checkTempDir()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Set workdir location if not defined in .conf file, doesn't exist, or not supported partition
    if (tempDirParent.isEmpty() || !QFile::exists(tempDirParent) || !FileSystemUtils::isOnSupportedPartition(tempDirParent)) {
        tempDirParent = FileSystemUtils::isOnSupportedPartition(snapshotDir) ? FileSystemUtils::largerFreeSpace("/tmp", "/home", snapshotDir)
                                                                             : FileSystemUtils::largerFreeSpace("/tmp", "/home");
    }
    if (tempDirParent == "/home") {
        QString userName = QString::fromUtf8(qgetenv("SUDO_USER")).trimmed();
        if (userName.isEmpty()) {
            userName = QString::fromUtf8(qgetenv("LOGNAME")).trimmed();
        }
        tempDirParent = "/home/" + userName;
    }
    tmpdir.reset(new QTemporaryDir(tempDirParent + "/mx-snapshot-XXXXXXXX"));
    if (!tmpdir->isValid()) {
        qCritical() << QObject::tr("Could not create temp directory:") << tmpdir->path();
        qCritical() << QObject::tr("Please check that the parent directory exists and is writable:");
        qCritical() << tempDirParent;
        return false;
    }
    workDir = tmpdir->path();
    freeSpaceWork = FileSystemUtils::getFreeSpace(workDir);

    QDir().mkpath(workDir + "/iso-template/antiX");
    qDebug() << "Work directory is placed in" << tempDirParent;
    return true;
}

bool Settings::checkConfiguration() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    // Check compression format
    if (!checkCompression()) {
        qCritical() << QObject::tr("Compression format '%1' is not supported by the current kernel").arg(compression);
        return false;
    }

    // Check cores setting
    if (cores == 0 || cores > maxCores) {
        qCritical() << QObject::tr("Invalid cores setting: %1. Must be between 1 and %2").arg(cores).arg(maxCores);
        return false;
    }

    // Check throttle setting
    if (throttle > 20) {
        qCritical() << QObject::tr("Invalid throttle setting: %1. Must be between 0 and 20").arg(throttle);
        return false;
    }

    // Check snapshot directory
    if (snapshotDir.isEmpty()) {
        qCritical() << QObject::tr("Snapshot directory cannot be empty");
        return false;
    }

    // Note: Directory creation is handled later with elevated permissions in checkSnapshotDir()

    // Check snapshot name
    if (snapshotName.isEmpty()) {
        qCritical() << QObject::tr("Snapshot name cannot be empty");
        return false;
    }

    // Check for invalid characters in snapshot name
    if (snapshotName.contains(QRegularExpression("[<>:\"/\\|?*]"))) {
        qCritical() << QObject::tr("Snapshot name contains invalid characters: %1").arg(snapshotName);
        return false;
    }

    // Check kernel
    if (kernel.isEmpty()) {
        qCritical() << QObject::tr("Kernel version cannot be empty");
        return false;
    }

    if (!QFileInfo::exists("/boot/vmlinuz-" + kernel)) {
        qCritical() << QObject::tr("Kernel file not found: /boot/vmlinuz-%1").arg(kernel);
        return false;
    }

    // Check if SQUASHFS is available in kernel (skip on Arch)
    if (!isArch) {
        if (QProcess::execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", "/boot/config-" + kernel}) != 0) {
            qCritical() << QObject::tr("Kernel %1 doesn't support Squashfs").arg(kernel);
            return false;
        }
    }

    // Validate exclusions
    if (!validateExclusions()) {
        return false;
    }

    // Validate space requirements
    if (!validateSpaceRequirements()) {
        return false;
    }

    qDebug() << "Configuration validation passed";
    return true;
}

bool Settings::validateExclusions() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    // Check if exclusion file exists and is readable
    if (!snapshotExcludes.fileName().isEmpty() && !snapshotExcludes.exists()) {
        qCritical() << QObject::tr("Exclusion file does not exist: %1").arg(snapshotExcludes.fileName());
        return false;
    }

    // Validate session exclusions format
    if (!sessionExcludes.isEmpty()) {
        // Check for balanced quotes
        const int quoteCount = sessionExcludes.count('"');
        if (quoteCount % 2 != 0) {
            qCritical() << QObject::tr("Unbalanced quotes in exclusion list");
            return false;
        }
    }

    qDebug() << "Exclusion validation passed";
    return true;
}

bool Settings::validateSpaceRequirements() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    // Check if we have minimum free space (at least 1GB)
    constexpr quint64 MIN_FREE_SPACE = 1024 * 1024; // 1GB in KiB

    // Get free space for snapshot directory (or its parent if it doesn't exist)
    QString pathToCheck = snapshotDir;
    if (!QDir(snapshotDir).exists()) {
        // If snapshot dir doesn't exist, check parent directory
        pathToCheck = QFileInfo(snapshotDir).absolutePath();
    }

    const quint64 availableSpace = FileSystemUtils::getFreeSpace(pathToCheck);
    if (availableSpace < MIN_FREE_SPACE) {
        qCritical() << QObject::tr("Insufficient free space: %1 KiB available, minimum %2 KiB required")
                           .arg(availableSpace).arg(MIN_FREE_SPACE);
        return false;
    }

    // Only check work directory space if it has been initialized (checkTempDir() called)
    if (freeSpaceWork > 0 && freeSpaceWork < MIN_FREE_SPACE) {
        qCritical() << QObject::tr("Insufficient free space in work directory: %1 KiB available, minimum %2 KiB required")
                           .arg(freeSpaceWork).arg(MIN_FREE_SPACE);
        return false;
    }

    qDebug() << "Space requirements validation passed";
    return true;
}

bool Settings::initializeConfiguration()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    // Validate maxCores
    if (maxCores == 0) {
        qCritical() << QObject::tr("Failed to determine number of CPU cores");
        return false;
    }

    // Check if config file exists and is readable
    if (!configFile.exists()) {
        qWarning() << QObject::tr("Configuration file does not exist: %1").arg(configFile.fileName());
        qWarning() << QObject::tr("Using default settings");
    } else if (!configFile.open(QIODevice::ReadOnly)) {
        qCritical() << QObject::tr("Cannot read configuration file: %1").arg(configFile.fileName());
        qCritical() << QObject::tr("Error: %1").arg(configFile.errorString());
        return false;
    } else {
        configFile.close();
    }

    // Check for required system tools
    QStringList requiredTools = {"mksquashfs", "xorriso", "lslogins"};
    for (const QString &tool : requiredTools) {
        if (QProcess::execute("which", {tool}) != 0) {
            qCritical() << QObject::tr("Required tool not found: %1").arg(tool);
            return false;
        }
    }

    // Check for required directories
    QStringList requiredDirs = {"/boot", "/etc", "/usr/lib"};
    for (const QString &dir : requiredDirs) {
        if (!QDir(dir).exists()) {
            qCritical() << QObject::tr("Required directory not found: %1").arg(dir);
            return false;
        }
    }

    qDebug() << "Configuration initialization passed";
    return true;
}

void Settings::handleInitializationError(const QString &error) const
{
    qCritical() << "Settings initialization error:" << error;

    // Log to system log if available
    if (QFile::exists("/usr/bin/logger")) {
        QProcess::execute("logger", {"-t", qApp->applicationName(), "Settings initialization error: " + error});
    }

    // Show error message appropriate for current mode (GUI or CLI)
    MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Initialization Error"),
                               QObject::tr("Failed to initialize application settings:\n\n%1").arg(error));
}

QString Settings::getEditor() const
{
    QString editor = guiEditor;
    if (editor.isEmpty() || QStandardPaths::findExecutable(editor, {path}).isEmpty()) {
        const QString defaultEditor = Cmd().getOut("xdg-mime query default text/plain");
        const QString desktopFile
            = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, defaultEditor, QStandardPaths::LocateFile);
        QFile file(desktopFile);
        if (file.open(QIODevice::ReadOnly)) {
            while (!file.atEnd()) {
                QString line = file.readLine();
                if (line.contains(QRegularExpression("^Exec="))) {
                    editor = line.remove(QRegularExpression("^Exec=|%u|%U|%f|%F|%c|%C|-b")).trimmed();
                    break;
                }
            }
            file.close();
        }
        if (editor.isEmpty()) { // Use nano as backup editor
            editor = "nano";
        }
    }

    const bool isRoot = getuid() == 0;
    const bool isEditorThatElevates
        = QRegularExpression(R"((kate|kwrite|featherpad|code|codium)$)").match(editor).hasMatch();
    const bool isCliEditor = QRegularExpression(R"(nano|vi|vim|nvim|micro|emacs)").match(editor).hasMatch();

    QString elevate = Cmd::elevationTool();
    if (isEditorThatElevates && !isRoot) {
        return editor;
    } else if (isRoot && isEditorThatElevates) {
        // Adjust user switch flag based on tool
        if (elevate.contains("sudo")) {
            elevate += " -u $(logname)";
        } else {
            elevate += " --user $(logname)";
        }
    }
    if (isCliEditor) {
        return "x-terminal-emulator -e " + elevate + " " + editor;
    }
    return elevate + " env DISPLAY=$DISPLAY XAUTHORITY=$XAUTHORITY " + editor;
}

// Return the size of the snapshot folder in MiB
QString Settings::getSnapshotSize() const
{
    qint64 totalSize = 0;
    if (QFileInfo::exists(snapshotDir)) {
        QDir directory(snapshotDir);
        QStringList isoFiles = directory.entryList(QStringList() << "*.iso", QDir::Files);
        for (const QString &file : isoFiles) {
            totalSize += QFileInfo(directory.absoluteFilePath(file)).size();
        }
    }
    return QString::number(totalSize / (1024 * 1024)) + "MiB";
}

// Number of snapshots in snapshot_dir
int Settings::getSnapshotCount() const
{
    if (QFileInfo::exists(snapshotDir)) {
        QFileInfoList list = QDir(snapshotDir).entryInfoList(QStringList("*.iso"), QDir::Files);
        return list.size();
    }
    return 0;
}



// Return the XDG User Directory for each user with different localizations than English
QString Settings::getXdgUserDirs(const QString &folder)
{
    QStringList resultParts;
    resultParts.reserve(18); // For 3 users x 6 folders, not worth getting the number of users on the system

    const static QHash<QString, QString> englishDirs {
        {"DOCUMENTS", "Documents"}, {"DOWNLOAD", "Downloads"}, {"DESKTOP", "Desktop"},
        {"MUSIC", "Music"},         {"PICTURES", "Pictures"},  {"VIDEOS", "Videos"},
    };
    for (const QString &user : std::as_const(users)) {
        QString dir = Cmd().getOutAsRoot("runuser " + user + " -c \"xdg-user-dir " + folder + "\" 2>/dev/null");
        if (!dir.isEmpty() && englishDirs.value(folder) != dir.section('/', -1) && dir != "/home/" + user
            && dir != "/home/" + user + '/') {
            dir.remove(QRegularExpression("^/"));
            QString exclusion = folder == "DESKTOP" ? "/!(minstall.desktop)" : "/*\" \"" + dir + "/.*";
            dir.append(exclusion);
            resultParts << dir;
        }
    }
    QString result = resultParts.join("\" \"");
    return result.isEmpty() ? QString() : "\" \"" + result;
}

void Settings::selectKernel()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    kernel.remove(QRegularExpression("^/boot/vmlinuz-"));
    if (kernel.isEmpty() || !QFileInfo::exists("/boot/vmlinuz-" + kernel)) {
        kernel = currentKernel;
        if (!QFileInfo::exists("/boot/vmlinuz-" + kernel)) {
            QDir directory("/boot");
            QStringList vmlinuzFiles = directory.entryList(QStringList() << "vmlinuz-*", QDir::Files, QDir::Name);
            if (!vmlinuzFiles.isEmpty()) {
                kernel = vmlinuzFiles.last().remove(QRegularExpression("^vmlinuz-"));
            }
            if (!QFileInfo::exists("/boot/vmlinuz-" + kernel)) {
                QString message = QObject::tr("Could not find a usable kernel");
                QString details = QObject::tr("Searched for kernel files in /boot/ but none were found or accessible.");
                MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message + "\n\n" + details);
                exit(EXIT_FAILURE);
            }
        }
    }
    // Check if SQUASHFS is available unless running on Arch (config may be unavailable)
    if (!isArch) {
        if (QProcess::execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", "/boot/config-" + kernel}) != 0) {
            QString message = QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
            MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message);
            exit(EXIT_FAILURE);
        }
    }
}

void Settings::setVariables()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    try {
        // live and users are now const members initialized in constructor
        if (users.isEmpty()) {
            qWarning() << QObject::tr("No users found in the system");
        }
    } catch (...) {
        qCritical() << QObject::tr("Failed to determine system information");
        throw;
    }

    QString distroVersionFile;
    if (QFileInfo::exists("/etc/mx-version")) {
        distroVersionFile = "/etc/mx-version";
    } else if (QFileInfo::exists("/etc/antix-version")) {
        distroVersionFile = "/etc/antix-version";
    }

    const QString osName = readOsReleaseValue("NAME");
    const QString osVersionId = readOsReleaseValue("VERSION_ID");
    const QString osVersionCodename = readOsReleaseValue("VERSION_CODENAME");
#ifdef ARCH_BUILD
    isArch = true;
#else
    isArch = false;
#endif

    if (QFileInfo::exists("/etc/lsb-release")) {
        projectName = Cmd().getOut("grep -oP '(?<=DISTRIB_ID=).*' /etc/lsb-release");
    } else if (!osName.isEmpty()) {
        projectName = osName;
    } else {
        projectName = Cmd().getOut("lsb_release -i | cut -f2");
    }
    projectName.replace('"', "");
    if (!distroVersionFile.isEmpty()) {
        distroVersion = Cmd().getOut("cut -f1 -d'_' " + distroVersionFile);
        distroVersion.remove(QRegularExpression("^" + projectName + "_|^" + projectName + "-"));
    } else if (!osVersionId.isEmpty()) {
        distroVersion = osVersionId;
    } else {
        distroVersion = Cmd().getOut("lsb_release -r | cut -f2");
    }
    if (isArch) {
        fullDistroName = distroVersion + "_" + QString(x86 ? "386" : "x64");
    } else {
        fullDistroName = projectName + "-" + distroVersion + "_" + QString(x86 ? "386" : "x64");
    }
    releaseDate = QDate::currentDate().toString("MMMM dd, yyyy");
    if (QFileInfo::exists("/etc/lsb-release")) {
        codename = Cmd().getOut("grep -oP '(?<=DISTRIB_CODENAME=).*' /etc/lsb-release");
    } else if (!osVersionCodename.isEmpty()) {
        codename = osVersionCodename;
    } else if (isArch) {
        codename = "rolling";
    } else {
        codename = Cmd().getOut("lsb_release -c | cut -f2");
    }
    codename.replace('"', "");
    bootOptions = monthly ? "quiet splasht nosplash" : SystemInfo::readKernelOpts();
}

QString Settings::getFilename() const
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (stamp == "datetime") {
        return snapshotBasename + "-" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmm") + ".iso";
    } else {
        QString name;
        QDir dir;
        int n = 1;
        do {
            name = snapshotBasename + QString::number(n) + ".iso";
            dir.setPath('"' + snapshotDir + '/' + name + '"');
            n++;
        } while (QFileInfo::exists(dir.absolutePath()));
        return name;
    }
}

quint64 Settings::getLiveRootSpace()
{
    // rootspaceneeded is the size of the linuxfs file * a compression factor + contents of the rootfs, conservative
    // but fast factors are same as used in live-remaster

    // Load some live variables
    QSettings livesettings("/live/config/initrd.out", QSettings::NativeFormat);
    QString sqfile_full = livesettings.value("SQFILE_FULL", "/live/boot-dev/antiX/linuxfs").toString();
    QString toram_mp = livesettings.value("TORAM_MP", "/live/to-ram").toString();
    QString sqfile_path = livesettings.value("SQFILE_PATH", "antiX").toString().remove(QRegularExpression("^/+"));
    QString sqfile_name = livesettings.value("SQFILE_NAME", "linuxfs").toString();
    if (!toram_mp.isEmpty() && QFileInfo::exists(toram_mp + "/" + sqfile_path + "/" + sqfile_name)) {
        sqfile_full = toram_mp + "/" + sqfile_path + "/" + sqfile_name;
    }

    // Get compression factor by reading the linuxfs squasfs file, if available
    QString linuxfs_compression_type
        = Cmd().getOut("dd if=" + sqfile_full + " bs=1 skip=20 count=2 status=none 2>/dev/null |od -An -tdI");
    constexpr quint8 default_factor = 30;
    quint8 c_factor = default_factor;
    // gzip, xz, or lz4
    QMap<QString, QString> compression_types
        = {{"1", "gzip"}, {"2", "lzo"}, {"3", "lzma"}, {"4", "xz"}, {"5", "lz4"}, {"6", "zstd"}};
    if (compression_types.contains(linuxfs_compression_type)) {
        c_factor = compressionFactor.value(compression_types.value(linuxfs_compression_type));
    } else {
        qWarning() << "Unknown compression type:" << linuxfs_compression_type;
    }
    quint64 rootfs_file_size = 0;
    quint64 linuxfs_file_size = QStorageInfo("/live/linux/").bytesTotal() * 100 / c_factor;
    if (QFileInfo::exists("/live/persist-root")) {
        rootfs_file_size = QStorageInfo("/live/persist-root/").bytesTotal();
    }

    // Add rootfs file size to the calculated linuxfs file size.  probaby conservative, as rootfs will likely have some
    // overlap with linuxfs
    return linuxfs_file_size + rootfs_file_size;
}

QString Settings::getUsedSpace()
{
    constexpr double factor = 1024 * 1024 * 1024;
    QString out = "\n- " + QObject::tr("Used space on / (root): ");
    if (live) {
        rootSize = getLiveRootSpace();
        out += QString::number(static_cast<double>(rootSize) / factor, 'f', 2) + "GiB -- " + QObject::tr("estimated");
    } else {
        QStorageInfo rootInfo("/");
        rootSize = rootInfo.bytesTotal() - rootInfo.bytesFree();
        out += QString::number(static_cast<double>(rootSize) / factor, 'f', 2) + "GiB";
    }
    QStorageInfo homeInfo("/home/");
    if (homeInfo.isValid() && homeInfo.isRoot()) {
        homeSize = homeInfo.bytesTotal() - homeInfo.bytesFree();
        out.append("\n- " + QObject::tr("Used space on /home: ")
                   + QString::number(static_cast<double>(homeSize) / factor, 'f', 2) + "GiB");
    } else {
        homeSize = 0; // /home on root
    }
    return out;
}



int Settings::getDebianVerNum()
{
    QFile file("/etc/debian_version");
    if (!file.exists()) {
        return Release::Bookworm;
    }
    QStringList list;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        list = line.split('.');
        file.close();
    } else {
        return Release::Bookworm;
    }
    bool ok = false;
    int ver = list.at(0).toInt(&ok);
    if (ok) {
        return ver;
    } else {
        QString verName = list.at(0).split('/').at(0);
        if (verName == "bullseye") {
            return Release::Bullseye;
        } else if (verName == "bookworm") {
            return Release::Bookworm;
        } else if (verName == "trixie") {
            return Release::Trixie;
        } else if (verName == "forky") {
            return Release::Forky;
        } else if (verName == "duke") {
            return Release::Duke;
        } else {
            qCritical() << "Unknown Debian version:" << ver << "Assumes Bullseye";
            return Release::Bullseye;
        }
    }
}





QString Settings::getFreeSpaceStrings(const QString &path)
{
    constexpr float factor = 1024 * 1024;
    freeSpace = FileSystemUtils::getFreeSpace(path);
    QString out = QString::number(static_cast<double>(freeSpace) / factor, 'f', 2) + "GiB";

    qDebug().noquote() << QString("- " + QObject::tr("Free space on %1, where snapshot folder is placed: ").arg(path)
                                  + out)
                       << '\n';

    qDebug().noquote() << QObject::tr(
                              "The free space should be sufficient to hold the compressed data from / and /home\n\n"
                              "      If necessary, you can create more available space\n"
                              "      by removing previous snapshots and saved copies:\n"
                              "      %1 snapshots are taking up %2 of disk space.\n")
                              .arg(QString::number(getSnapshotCount()), getSnapshotSize());
    return out;
}



void Settings::excludeItem(const QString &item)
{
    static const QMap<QString, void (Settings::*)(bool)> itemExclusions {
        {QObject::tr("Desktop"), &Settings::excludeDesktop},
        {QObject::tr("Documents"), &Settings::excludeDocuments},
        {QObject::tr("Downloads"), &Settings::excludeDownloads},
        {QObject::tr("Flatpaks"), &Settings::excludeFlatpaks},
        {QObject::tr("Music"), &Settings::excludeMusic},
        {QObject::tr("Networks"), &Settings::excludeNetworks},
        {QObject::tr("Pictures"), &Settings::excludePictures},
        {"Steam", &Settings::excludeSteam},
        {QObject::tr("Videos"), &Settings::excludeVideos},
        {"VirtualBox", &Settings::excludeVirtualBox}};

    auto it = itemExclusions.find(item);
    if (it != itemExclusions.end()) {
        (this->*(it.value()))(true);
    }
}

void Settings::excludeDesktop(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Desktop);
    }
    QString exclusion = "/home/*/Desktop/!(minstall.desktop)" + getXdgUserDirs("DESKTOP");
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeDocuments(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Documents);
    }
    QString folder {"home/*/Documents/"};
    QString xdg_name {"DOCUMENTS"};
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeDownloads(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Downloads);
    }
    QString folder {"home/*/Downloads/"};
    QString xdg_name {"DOWNLOAD"};
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeFlatpaks(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Flatpaks);
    }
    QString exclusion = "home/*/.local/share/flatpak/*\" \"home/*/.local/share/flatpak/.*\" "
                        "\"var/lib/flatpak/*\" \"var/lib/flatpak/.*\" "
                        "\"home/*/.var/app/*\" \"home/*/.var/app/.*";
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeMusic(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Music);
    }
    QString folder {"home/*/Music/"};
    QString xdg_name {"MUSIC"};
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeNetworks(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Networks);
    }
    addRemoveExclusion(exclude, QStringLiteral("/etc/NetworkManager/system-connections/*"));
    addRemoveExclusion(exclude, QStringLiteral("/etc/wicd/*"));
    addRemoveExclusion(exclude, QStringLiteral("/var/lib/connman/*"));
}

void Settings::excludePictures(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Pictures);
    }
    QString folder {"home/*/Pictures/"};
    QString xdg_name {"PICTURES"};
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeSteam(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Steam);
    }
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
    QByteArray content = file.readAll();
    QStringList lines = QString::fromUtf8(content).split('\n');

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith('/') && !trimmedLine.startsWith("/dev/")) {
            QStringList parts = trimmedLine.split(QRegularExpression("\\s+"));
            if (parts.size() > 3 && parts.at(2) == "swap") {
                addRemoveExclusion(true, parts[0].remove(0, 1));
            }
        }
    }
}

void Settings::excludeVideos(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::Videos);
    }
    QString folder {"home/*/Videos/"};
    QString xdg_name {"VIDEOS"};
    QString exclusion = folder + "*\" \"" + folder + ".*" + getXdgUserDirs(xdg_name);
    addRemoveExclusion(exclude, exclusion);
}

void Settings::excludeVirtualBox(bool exclude)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (exclude) {
        exclusions.setFlag(Exclude::VirtualBox);
    }
    addRemoveExclusion(exclude, QStringLiteral("home/*/VirtualBox VMs"));
}

// Load settings from config file
void Settings::loadConfig()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";

    QSettings settingsSystem(configFile.fileName(), QSettings::IniFormat);
    if (settingsSystem.status() != QSettings::NoError) {
        qWarning() << QObject::tr("Error reading system configuration file: %1").arg(configFile.fileName());
    }

    // Ensure we use the logged-in user's config location even when running under sudo/root
    const QString configDir = userConfigBaseDir();
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, configDir);
    QSettings settingsUser;
    if (settingsUser.status() != QSettings::NoError) {
        qWarning() << QObject::tr("Error accessing user configuration");
    }

    const QString systemSnapshotExcludes = trimQuotes(settingsSystem.value("snapshot_excludes").toString());
    const bool userConfiguredSnapshotExcludes = settingsUser.contains("snapshot_excludes");

    // Read all keys from system settings
    settingsSystem.beginGroup("");
    QStringList systemKeys = settingsSystem.allKeys();
    settingsSystem.endGroup();

    // Merge system settings into user settings
    foreach (const QString &key, systemKeys) {
        if (!settingsUser.contains(key)) {
            QVariant value = settingsSystem.value(key);
            settingsUser.setValue(key, value);
        }
    }

    sessionExcludes.clear();
    snapshotDir = trimQuotes(settingsUser.value("snapshot_dir", "/home/snapshot").toString());
    if (!snapshotDir.endsWith("/snapshot")) {
        snapshotDir = QDir::cleanPath(snapshotDir + "/snapshot");
    }
    const QString userConfigDir = QDir::cleanPath(configDir + "/" + QCoreApplication::organizationName());
    const QString userExcludesPath =
        QDir::cleanPath(userConfigDir + "/" + qApp->applicationName() + "-exclude.list");
    const QString userConfigPath = settingsUser.fileName();
    const QString systemExcludesPath = QDir::cleanPath("/etc/" + qApp->applicationName() + "-exclude.list");
    QString localPath = QDir::cleanPath("/usr/local/share/excludes/" + qApp->applicationName() + "-exclude.list");
    QString usrPath = QDir::cleanPath("/usr/share/excludes/" + qApp->applicationName() + "-exclude.list");
    const QString fallbackExcludesPath = QFileInfo::exists(localPath) ? localPath : usrPath;
    excludesSourcePath = QFileInfo::exists(systemExcludesPath) ? systemExcludesPath : fallbackExcludesPath;
    QString configuredExcludesPath = trimQuotes(settingsUser.value("snapshot_excludes", userExcludesPath).toString());

    if (!userConfiguredSnapshotExcludes || configuredExcludesPath == systemSnapshotExcludes) {
        configuredExcludesPath = userExcludesPath;
        settingsUser.setValue("snapshot_excludes", configuredExcludesPath);
    }

    const bool usingDefaultUserPath = configuredExcludesPath == userExcludesPath;
    if (usingDefaultUserPath && !QFileInfo::exists(userExcludesPath)) {
        if (!excludesSourcePath.isEmpty() && QFileInfo::exists(excludesSourcePath)) {
            QDir().mkpath(userConfigDir);
            if (QFile::copy(excludesSourcePath, userExcludesPath)) {
                qDebug() << "Copied exclusion file from" << excludesSourcePath << "to" << userExcludesPath;
                const QString username = loggedInUserName();
                if (!username.isEmpty()) {
                    const QString chownCmd = QStringLiteral("chown %1: \"%2\"").arg(username, userExcludesPath);
                    Cmd().runAsRoot(chownCmd, Cmd::QuietMode::Yes);
                }
            } else {
                qWarning() << QObject::tr("Could not copy exclusion file from %1 to %2")
                                  .arg(excludesSourcePath, userExcludesPath);
                configuredExcludesPath = excludesSourcePath;
            }
        }
    }
    if (!QFileInfo::exists(configuredExcludesPath)) {
        qDebug() << "Configured snapshot_excludes file not found (" << configuredExcludesPath
                 << "), using fallback path:" << fallbackExcludesPath;
        configuredExcludesPath = fallbackExcludesPath;
    }
    snapshotExcludes.setFileName(configuredExcludesPath);
    chownFileToLoggedInUser(userConfigPath);
    chownFileToLoggedInUser(configuredExcludesPath);
    // snapshotBasename, makeIsohybrid, guiEditor, stamp, forceInstaller are now const members
    makeMd5sum = settingsUser.value("make_md5sum", "no").toString() != "no";
    makeSha512sum = settingsUser.value("make_sha512sum", "no").toString() != "no";
    compression = trimQuotes(settingsUser.value("compression", "zstd").toString());
    mksqOpt = trimQuotes(settingsUser.value("mksq_opt").toString());
    // edit_boot_menu is now const, initialized in constructor
    tempDirParent = trimQuotes(settingsUser.value("workdir").toString());

    const QVariant coresValue = settingsUser.value("cores", maxCores);
    uint storedCores = coresValue.toUInt();
    if (storedCores == 0 || storedCores > maxCores) {
        qWarning() << QObject::tr("Invalid stored cores setting (%1). Using detected CPU count: %2")
                          .arg(coresValue.toString()).arg(maxCores);
        storedCores = maxCores;
        settingsUser.setValue("cores", storedCores);
    }
    cores = storedCores;

    throttle = settingsUser.value("throttle", 0).toUInt();
    resetAccounts = false;
}

void Settings::excludeAll()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    excludeDesktop(true);
    excludeDocuments(true);
    excludeDownloads(true);
    excludeFlatpaks(true);
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
    // Add exclusions snapshot and work dirs
    addRemoveExclusion(true, snapshotDir);
    addRemoveExclusion(true, workDir);

    if (resetAccounts) {
        addRemoveExclusion(true, QStringLiteral("/etc/minstall.conf"));
#ifndef ARCH_BUILD
        // Exclude /etc/localtime if link and timezone not America/New_York
        QFileInfo localtimeInfo("/etc/localtime");
        QFile timezoneFile("/etc/timezone");
        if (localtimeInfo.isSymLink()) {
            if (timezoneFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&timezoneFile);
                QString timezone = in.readLine();
                if (timezone != "America/New_York") {
                    addRemoveExclusion(true, "/etc/localtime");
                }
                timezoneFile.close();
            }
        }
#endif
    }
    excludeSwapFile();
}

void Settings::processArgs(const QCommandLineParser &argParser)
{
    shutdown = argParser.isSet("shutdown");
    const QString kernelArg = argParser.value("kernel");
    if (!kernelArg.isEmpty()) {
        kernel = kernelArg;
    }
    // preempt is now const, initialized in constructor
    QDir dir;
    if (!argParser.value("directory").isEmpty()) {
        const QString directory = argParser.value("directory");
        if (QFileInfo::exists(directory)) {
            dir.setPath(directory);
            snapshotDir = dir.absolutePath() + "/snapshot";
        } else {
            qWarning() << "Warning: Specified directory does not exist:" << directory;
            qWarning() << "Using default snapshot directory:" << snapshotDir;
        }
    }

    if (!argParser.value("workdir").isEmpty()) {
        const QString workdir = argParser.value("workdir");
        if (QFileInfo::exists(workdir)) {
            dir.setPath(workdir);
            tempDirParent = dir.absolutePath();
        } else {
            qWarning() << "Warning: Specified work directory does not exist:" << workdir;
            qWarning() << "Using default work directory:" << tempDirParent;
        }
    }

    if (!argParser.value(QStringLiteral("file")).isEmpty()) {
        const auto fileArg = argParser.value("file");
        snapshotName = fileArg + (fileArg.endsWith(".iso") ? QString() : ".iso");
    } else {
        snapshotName = getFilename();
    }
    if (QFile::exists(snapshotDir + '/' + snapshotName)) {
        QString message
            = QObject::tr("Output file %1 already exists. Please use another file name, or delete the existent file.")
                  .arg(snapshotDir + '/' + snapshotName);
        MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message);
        exit(EXIT_FAILURE);
    }
    resetAccounts = argParser.isSet("reset");
    if (resetAccounts) {
        excludeAll();
    }
    if (argParser.isSet("month")) {
        resetAccounts = true;
    }
    if (argParser.isSet("checksums")) {
        makeSha512sum = makeMd5sum = true;
    }
    if (argParser.isSet("month")) {
        makeSha512sum = true;
        makeMd5sum = false;
    }
    if (argParser.isSet("no-checksums")) {
        makeSha512sum = makeMd5sum = false;
    }
    if (!argParser.value("compression").isEmpty()) {
        compression = argParser.value("compression");
    }
    if (!argParser.value("compression-level").isEmpty()) {
        mksqOpt = argParser.value("compression-level");
    }
    if (!argParser.value("cores").isEmpty()) {
        bool ok {false};
        const uint val = argParser.value("cores").toUInt(&ok);
        if (!ok || val == 0 || val > maxCores) {
            qWarning() << "Invalid cores value:" << argParser.value("cores")
                       << "- must be between 1 and" << maxCores;
            qWarning() << "Using default:" << cores;
        } else {
            cores = val;
        }
    }
    if (!argParser.value("throttle").isEmpty()) {
        bool ok {false};
        const uint val = argParser.value("throttle").toUInt(&ok);
        if (!ok || val > 99) {
            qWarning() << "Invalid throttle value:" << argParser.value("throttle")
                       << "- must be between 0 and 99";
            qWarning() << "Using default:" << throttle;
        } else {
            throttle = val;
        }
    }
    selectKernel();
}

void Settings::processExclArgs(const QCommandLineParser &argParser)
{
    static const QSet<QString> valid_options {"Desktop",  "Documents", "Downloads", "Flatpaks", "Music",
                                              "Networks", "Pictures",  "Steam",     "Videos",   "VirtualBox"};
    if (argParser.isSet("exclude")) {
        const QStringList options = argParser.values("exclude");
        for (const QString &option : options) {
            if (valid_options.contains(option)) {
                excludeItem(option);
            } else {
                qDebug() << "Unknown option:" << option << '\n'
                         << "Please use one of these options" << valid_options.values();
            }
        }
    }
}



void Settings::setMonthlySnapshot(const QCommandLineParser &argParser)
{
    QString name;
    if (QFileInfo::exists(QStringLiteral("/etc/mx-version"))) {
        name = Cmd().getOut("cat /etc/mx-version |cut -f1 -d' '");
    } else {
        qDebug() << "/etc/mx-version not found. Not MX Linux?";
        name = "MX_" + QString(x86 ? "386" : "x64");
    }
    if (argParser.value("file").isEmpty()) {
        auto month = QDate::currentDate().toString("MMMM");
        if (!argParser.value("month").isEmpty()) {
            month += "." + argParser.value("month");
        }
        auto suffix = name.section('_', 1, 1);
        if (qgetenv("DESKTOP_SESSION") == "plasma") {
            suffix = "KDE";
        }
        snapshotName = name.section('_', 0, 0) + '_' + month + '_' + suffix + ".iso";
    }
    if (QFile::exists(snapshotDir + '/' + snapshotName)) {
        QString message
            = QObject::tr("Output file %1 already exists. Please use another file name, or delete the existent file.")
                  .arg(snapshotDir + '/' + snapshotName);
        MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message);
        exit(EXIT_FAILURE);
    }
    if (argParser.value("compression").isEmpty()) {
        compression = "zstd";
    }
    resetAccounts = true;
    bootOptions = "quiet splasht nosplash";
    excludeAll();
}

// Helper functions for const member initialization
QString Settings::getInitialKernel(const QCommandLineParser &argParser)
{
    QString kernelValue = argParser.value("kernel");

    // Remove path prefix if present
    kernelValue.remove(QRegularExpression("^/boot/vmlinuz-"));

    // If no kernel specified or invalid, use current kernel
    if (kernelValue.isEmpty() || !QFileInfo::exists("/boot/vmlinuz-" + kernelValue)) {
        kernelValue = currentKernel;

        // If current kernel doesn't exist, find the latest one
        if (!QFileInfo::exists("/boot/vmlinuz-" + kernelValue)) {
            QDir directory("/boot");
            QStringList vmlinuzFiles = directory.entryList(QStringList() << "vmlinuz-*", QDir::Files, QDir::Name);
            if (!vmlinuzFiles.isEmpty()) {
                kernelValue = vmlinuzFiles.last().remove(QRegularExpression("^vmlinuz-"));
            }
        }
    }

    return kernelValue;
}

bool Settings::getEditBootMenuSetting()
{
    QSettings settingsUser;
    if (settingsUser.contains("edit_boot_menu")) {
        return settingsUser.value("edit_boot_menu").toString() != "no";
    }

    QSettings settingsSystem("/etc/" + qApp->applicationName() + ".conf", QSettings::IniFormat);
    return settingsSystem.value("edit_boot_menu", "no").toString() != "no";
}

QString Settings::trimQuotes(const QString &value) const
{
    QString trimmed = value;

    // Remove leading and trailing whitespace first
    trimmed = trimmed.trimmed();

    // Remove single quotes if they surround the entire value
    if (trimmed.startsWith('\'') && trimmed.endsWith('\'') && trimmed.length() > 1) {
        trimmed = trimmed.mid(1, trimmed.length() - 2);
    }

    // Remove double quotes if they surround the entire value
    if (trimmed.startsWith('"') && trimmed.endsWith('"') && trimmed.length() > 1) {
        trimmed = trimmed.mid(1, trimmed.length() - 2);
    }

    return trimmed;
}

Settings::InitialSettings Settings::getInitialSettings() const
{
    QSettings settingsSystem("/etc/" + qApp->applicationName() + ".conf", QSettings::IniFormat);
    QSettings settingsUser;

    // Merge system settings into user settings
    settingsSystem.beginGroup("");
    QStringList systemKeys = settingsSystem.allKeys();
    settingsSystem.endGroup();

    foreach (const QString &key, systemKeys) {
        if (!settingsUser.contains(key)) {
            QVariant value = settingsSystem.value(key);
            settingsUser.setValue(key, value);
        }
    }

    return {
        .live = SystemInfo::isLive(),
        .forceInstaller = settingsUser.value("force_installer", true).toBool(),
        .makeIsohybrid = settingsUser.value("make_isohybrid", "yes").toString() == "yes",
        .guiEditor = trimQuotes(settingsUser.value("gui_editor").toString()),
        .snapshotBasename = trimQuotes(settingsUser.value("snapshot_basename", "snapshot").toString()),
        .stamp = trimQuotes(settingsUser.value("stamp").toString()),
        .users = SystemInfo::listUsers()
    };
}
