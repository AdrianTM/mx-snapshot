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

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QTemporaryFile>
#include <QTextStream>

#include <algorithm>
#include <stdexcept>

#include "bindrootmanager.h"
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
// For installed systems we account for /home when on a separate partition.
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
    // Allow common package name characters and optional architecture qualifiers like ":amd64".
    static const QRegularExpression validPackageName("^[a-z0-9+_.:-]+$");
    if (!validPackageName.match(package).hasMatch()) {
        qWarning() << "Invalid package name:" << package;
        return false;
    }

#ifdef ARCH_BUILD
    {
        QProcess pacmanQuery;
        pacmanQuery.start("pacman", {"-Q", package});
        if (!pacmanQuery.waitForFinished(5000)) {
            pacmanQuery.kill();
            pacmanQuery.waitForFinished(1000);
            return false;
        }
        return pacmanQuery.exitStatus() == QProcess::NormalExit && pacmanQuery.exitCode() == 0;
    }
#endif

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
    const QString elevateTool = Cmd::elevationTool();
    Cmd().run(elevateTool + " " + snapshotLib + " chown_conf", Cmd::QuietMode::Yes);
    if (!started) {
        shell.close();
        initrd_dir.remove();
        cleanupBindRootOverlay();
        exit(EXIT_SUCCESS);
    }
    emit message(tr("Cleaning..."));
    QTextStream out(stdout);
    out << "\033[?25h";
    out.flush();
    Cmd().run(elevateTool + " " + snapshotLib + " kill_mksquashfs", Cmd::QuietMode::Yes);
    shell.close();
    QProcess::execute("sync", {});
    QDir::setCurrent("/");
    if (BindRootManager::hasCleanupState()) {
        BindRootManager bindManager(shell, bindRootPath, settings->workDir + "/bind-root-work");
        if (!bindManager.cleanup()) {
            qWarning() << "Failed to cleanup bind-root state during shutdown.";
        }
    }
    cleanupBindRootOverlay();
    if (!settings->workDir.isEmpty()) {
        const QString workDirPath = QDir::cleanPath(settings->workDir);
        const QString baseName = QFileInfo(workDirPath).fileName();
        if (baseName.startsWith("mx-snapshot-") && QFileInfo::exists(workDirPath)) {
            shell.runAsRoot("rm -rf \"" + workDirPath + "\"", Cmd::QuietMode::Yes);
        }
    }
    initrd_dir.remove();
    settings->tmpdir.reset();
    if (done) {
        emit message(tr("Done"));
        Cmd().run(elevateTool + " " + snapshotLib + " copy_log", Cmd::QuietMode::Yes);
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
        Cmd().run(elevateTool + " " + snapshotLib + " copy_log", Cmd::QuietMode::Yes);
        exit(EXIT_FAILURE);
    }
}

// Check if we can put work_dir on another partition with enough space, move work_dir there and setupEnv again
bool Work::checkAndMoveWorkDir(const QString &dir, quint64 req_size)
{
    // See first if the dir is on different partition otherwise it's irrelevant
    if (QStorageInfo(dir + "/").device() != QStorageInfo(settings->snapshotDir + "/").device()
        && FileSystemUtils::getFreeSpace(dir) > req_size) {
        if (BindRootManager::hasCleanupState()) {
            BindRootManager bindManager(shell, bindRootPath, settings->workDir + "/bind-root-work");
            if (!bindManager.cleanup()) {
                qWarning() << "Failed to cleanup bind-root state while moving work dir.";
            }
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
    constexpr double kibToMib = 1024.0;
    qDebug().noquote() << "Needed space:" << QString::number(needed_space / kibToMib, 'f', 2) << "MiB";
    qDebug().noquote() << "Free space  :" << QString::number(free_space / kibToMib, 'f', 2) << "MiB on" << dir;
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

bool Work::setupBindRootOverlay()
{
    const QString rootFsType = Cmd().getOut("findmnt -n -o FSTYPE /", Cmd::QuietMode::Yes).trimmed();
    if (rootFsType == "overlay") {
        qDebug() << "Root filesystem is overlay; skipping bind-root overlay setup.";
        bindRootPath = "/.bind-root";
        bindRootOverlayActive = false;
        bindRootOverlayBase.clear();
        return true;
    }

    const QString appName = QCoreApplication::applicationName();
    const QString overlayBase = "/run/" + appName + "/bind-root-overlay";
    const QString lowerDir = overlayBase + "/lower";
    const QString upperDir = overlayBase + "/upper";
    const QString workDir = overlayBase + "/work";
    const QString bindRoot = overlayBase + "/root";

    const auto runRoot = [this](const QString &cmd) { return shell.runAsRoot(cmd, Cmd::QuietMode::Yes); };

    bindRootOverlayActive = false;
    bindRootOverlayBase.clear();
    bindRootPath = "/.bind-root";

    runRoot("mkdir -p \"" + overlayBase + "\" \"" + lowerDir + "\" \"" + upperDir + "\" \"" + workDir + "\" \""
            + bindRoot + "\"");

    if (runRoot("mountpoint -q \"" + bindRoot + "\"")) {
        runRoot("umount --recursive \"" + bindRoot + "\"");
    }
    if (runRoot("mountpoint -q \"" + lowerDir + "\"")) {
        runRoot("umount --recursive \"" + lowerDir + "\"");
    }

    if (!runRoot("mount --bind / \"" + lowerDir + "\"")) {
        qWarning() << "Failed to bind mount / to" << lowerDir;
        return false;
    }

    const QString cmd = "mount -t overlay overlay -o lowerdir=\"" + lowerDir + "\",upperdir=\"" + upperDir
                        + "\",workdir=\"" + workDir + "\" \"" + bindRoot + "\"";
    if (!runRoot(cmd)) {
        qWarning() << "Failed to mount overlay at" << bindRoot;
        runRoot("umount --recursive \"" + lowerDir + "\"");
        return false;
    }

    bindRootPath = bindRoot;
    bindRootOverlayBase = overlayBase;
    bindRootOverlayActive = true;
    return true;
}

void Work::cleanupBindRootOverlay()
{
    if (bindRootOverlayBase.isEmpty()) {
        bindRootOverlayActive = false;
        bindRootPath = "/.bind-root";
        return;
    }
    const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
    const QString elevateTool = Cmd::elevationTool();
    Cmd().run(elevateTool + " " + snapshotLib + " cleanup_overlay " + QCoreApplication::applicationName(),
              Cmd::QuietMode::Yes);
    bindRootOverlayActive = false;
    bindRootOverlayBase.clear();
    bindRootPath = "/.bind-root";
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
    shell.runAsRoot(
        QString("/usr/share/%1/scripts/copy-initrd-programs -e --to=\"%2\"").arg(qApp->applicationName(), to));
    shell.runAsRoot("chown -R $(logname): " + to);
}

// Copying the iso-template filesystem
void Work::copyNewIso()
{
    emit message(tr("Copying the new-iso filesystem..."));
    QDir::setCurrent(settings->workDir);

    QString templateTar = "/usr/lib/iso-template/iso-template.tar.gz";
    if (settings->isArch) {
        templateTar = "/usr/lib/iso-template/arch/iso-template.tar.gz";
        if (!QFileInfo::exists(templateTar)) {
            const QString fallback = QDir::homePath() + "/mx-iso-template/arch/iso-template.tar.gz";
            if (QFileInfo::exists(fallback)) {
                templateTar = fallback;
            }
        }
    }

    if (!QFileInfo::exists(templateTar)) {
        emit messageBox(BoxType::critical, tr("Error"), tr("ISO template not found: ") + templateTar);
        cleanUp();
    }

    if (settings->isArch) {
        QDir().mkpath("iso-template");
        shell.run("tar xf \"" + templateTar + "\" -C iso-template");
    } else {
        shell.run("tar xf \"" + templateTar + "\"");
    }

    if (settings->isArch) {
        const QString diskDirPath = settings->workDir + "/iso-template/.disk";
        QDir().mkpath(diskDirPath);
        QDir diskDir(diskDirPath);
        const QStringList oldUuids = diskDir.entryList(QStringList() << "*.uuid", QDir::Files);
        for (const QString &oldUuid : oldUuids) {
            diskDir.remove(oldUuid);
        }
        const QString uuidStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-00");
        QFile uuidFile(diskDir.filePath(uuidStamp + ".uuid"));
        if (!uuidFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to create .disk UUID file:" << uuidFile.fileName();
        }

        const QString antiXPath = settings->workDir + "/iso-template/antiX";
        if (QFileInfo::exists(antiXPath)) {
            QDir(antiXPath).removeRecursively();
        }

        const QString bootDir = settings->workDir + "/iso-template/boot";
        const QString efiDir = settings->workDir + "/iso-template/efi";
        if (!QFileInfo::exists(bootDir) || !QFileInfo::exists(efiDir)) {
            const QString wrongBoot = settings->workDir + "/boot";
            const QString wrongEfi = settings->workDir + "/efi";
            QString details = tr("Arch ISO template is missing boot/ or efi/ directories.");
            if (QFileInfo::exists(wrongBoot) || QFileInfo::exists(wrongEfi)) {
                details += "\n" + tr("Detected boot/ or efi/ under the work directory root; "
                                     "the template may have been extracted to the wrong location.");
            }
            details += "\n" + tr("Template: %1").arg(templateTar);
            emit messageBox(BoxType::critical, tr("Error"), details);
            cleanUp();
        }
        const QString archCpuDir = settings->x86 ? "i686" : "x86_64";
        const QString archBootDir = "iso-template/arch/boot/" + archCpuDir;
        const QString archBootPath = settings->workDir + "/" + archBootDir;
        const QString kernelPath = "/boot/vmlinuz-" + settings->kernel;
        QDir().mkpath(archBootDir);

        shell.runAsRoot("cp " + kernelPath + " \"" + archBootPath + "/vmlinuz-linux\"");
        shell.runAsRoot("chown $(logname): \"" + archBootPath + "/vmlinuz-linux\"");
        shell.runAsRoot("chmod a+r \"" + archBootPath + "/vmlinuz-linux\"");

        QString initramfsSource;
        const QString archisoPath = "/boot/archiso.img";
        if (QFileInfo::exists(archisoPath)) {
            QString archisoKernel = initramfsKernelVersion(archisoPath);
            const QString expectedKernel = kernelImageVersion(kernelPath);
            if (!expectedKernel.isEmpty()) {
                qDebug() << "Expected kernel from image:" << expectedKernel;
            } else {
                qWarning() << "Could not determine kernel version from" << kernelPath;
            }
            if (!archisoKernel.isEmpty()) {
                qDebug() << "archiso initramfs kernel:" << archisoKernel;
            }
            const bool needsRebuild = archisoKernel.isEmpty()
                || (!expectedKernel.isEmpty() && archisoKernel != expectedKernel);
            if (needsRebuild) {
                emit message(tr("Stale archiso initramfs detected, rebuilding..."));
                if (!rebuildArchisoInitramfs(archisoPath, kernelPath)) {
                    QString details = tr("Found /boot/archiso.img built for kernel %1, but the selected kernel is %2.")
                                          .arg(archisoKernel, expectedKernel.isEmpty() ? settings->kernel : expectedKernel);
                    details += "\n" + tr("Rebuilding /boot/archiso.img failed. Please rebuild it manually or remove the stale file.");
                    emit messageBox(BoxType::critical, tr("Error"), details);
                    cleanUp();
                }
                // Skip post-rebuild probing; mkinitcpio already verified the target kernel.
            }
            initramfsSource = archisoPath;
        } else if (QFileInfo::exists("/boot/initramfs-" + settings->kernel + ".img")) {
            initramfsSource = "/boot/initramfs-" + settings->kernel + ".img";
        } else if (QFileInfo::exists("/boot/initramfs-linux.img")) {
            initramfsSource = "/boot/initramfs-linux.img";
        }

        if (initramfsSource.isEmpty()) {
            emit messageBox(BoxType::critical, tr("Error"), tr("Could not find an initramfs image to use."));
            cleanUp();
        }

        shell.runAsRoot("cp \"" + initramfsSource + "\" \"" + archBootPath + "/archiso.img\"");
        shell.runAsRoot("chown $(logname): \"" + archBootPath + "/archiso.img\"");
        shell.runAsRoot("chmod a+r \"" + archBootPath + "/archiso.img\"");
    } else {
        shell.run("cp /usr/lib/iso-template/template-initrd.gz iso-template/antiX/initrd.gz");
        shell.run("cp /boot/vmlinuz-" + settings->kernel + " iso-template/antiX/vmlinuz");

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

    replaceMenuStrings();
}

// Create squashfs and then the iso
bool Work::createIso(const QString &filename)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Squash the filesystem copy
    QString unbuffer = checkInstalled("expect") ? "unbuffer " : "stdbuf -o0 ";
    using Release::Version;
    const QString archCpuDir = settings->x86 ? "i686" : "x86_64";
    const QString squashfsPath = settings->isArch
        ? settings->workDir + "/iso-template/arch/" + archCpuDir + "/airootfs.sfs"
        : settings->workDir + "/iso-template/antiX/linuxfs";
    if (settings->isArch) {
        QDir().mkpath(settings->workDir + "/iso-template/arch/" + archCpuDir);
    }
    const bool throttleSupported = settings->isArch
        ? Cmd().run("mksquashfs -help | grep -q -- -throttle", Cmd::QuietMode::Yes)
        : (Settings::getDebianVerNum() >= Version::Bookworm);
    const QString throttle = throttleSupported ? " -throttle " + QString::number(settings->throttle) : "";
    QString cmd = unbuffer + "mksquashfs \"" + bindRootPath + "\" \"" + squashfsPath + "\" -comp "
                  + settings->compression + " -processors " + QString::number(settings->cores) + throttle
                  + (settings->mksqOpt.isEmpty() ? "" : " " + settings->mksqOpt) + " -wildcards -ef "
                  + "\"" + settings->snapshotExcludes.fileName() + "\""
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

    if (settings->isArch) {
        const QString archIsoDir = "iso-2/arch/" + archCpuDir;
        QDir().mkpath(archIsoDir);
        shell.run("mv iso-template/arch/" + archCpuDir + "/airootfs.sfs* " + archIsoDir);
        makeChecksum(HashType::md5, settings->workDir + "/" + archIsoDir, "airootfs.sfs");
    } else {
        // Move linuxfs files to iso-2/antiX folder
        QDir().mkpath("iso-2/antiX");
        shell.run("mv iso-template/antiX/linuxfs* iso-2/antiX");
        makeChecksum(HashType::md5, settings->workDir + "/iso-2/antiX", "linuxfs");
    }

    BindRootManager bindManager(shell, bindRootPath, settings->workDir + "/bind-root-work");
    if (!bindManager.cleanup()) {
        qWarning() << "Failed to cleanup bind-root state after squashing.";
    }

    // Create the iso file
    QDir::setCurrent(settings->workDir + "/iso-template");
    const QString volumeLabel = settings->isArch ? settings->fullDistroName : "MXLIVE";
    if (settings->isArch) {
        const QString archAntiXPath = settings->workDir + "/iso-2/antiX";
        if (QFileInfo::exists(archAntiXPath)) {
            QDir(archAntiXPath).removeRecursively();
        }

        QString bootArgs;
        const QString biosBootRel = "boot/grub/i386-pc/eltorito.img";
        const QString biosBootPath = settings->workDir + "/iso-template/" + biosBootRel;
        if (QFileInfo::exists(biosBootPath)) {
            bootArgs += " -b " + biosBootRel + " -no-emul-boot -boot-load-size 4"
                        " -boot-info-table -c boot.catalog";
        } else {
            qWarning() << "Missing BIOS boot image:" << biosBootPath;
        }

        const QString efiBootRel = "efi.img";
        const QString efiBootPath = settings->workDir + "/iso-template/" + efiBootRel;
        if (QFileInfo::exists(efiBootPath)) {
            bootArgs += " -eltorito-alt-boot -e " + efiBootRel + " -no-emul-boot";
        } else {
            qWarning() << "Missing EFI boot image:" << efiBootPath;
        }

        QString isohybridArgs;
        bool useXorrisoHybrid = false;
        if (settings->makeIsohybrid) {
            static const QStringList mbrCandidates = {
                "/usr/lib/syslinux/bios/isohdpfx.bin",
                "/usr/lib/syslinux/isohdpfx.bin",
                "/usr/share/syslinux/isohdpfx.bin",
                "/usr/lib/ISOLINUX/isohdpfx.bin",
            };
            QString foundMbrPath;
            for (const QString &candidate : mbrCandidates) {
                if (QFileInfo::exists(candidate)) {
                    foundMbrPath = candidate;
                    break;
                }
            }
            if (!foundMbrPath.isEmpty()) {
                isohybridArgs = " -isohybrid-mbr \"" + foundMbrPath + "\" -isohybrid-gpt-basdat";
                useXorrisoHybrid = true;
                qDebug() << "Using isohybrid MBR:" << foundMbrPath;
            }
            if (!useXorrisoHybrid) {
                qWarning() << "isohdpfx.bin not found; skipping xorriso hybrid flags.";
            }
        }

        cmd = "xorriso -as mkisofs -l -V \"" + volumeLabel
              + "\" -R -J -pad -iso-level 3" + isohybridArgs + " -no-emul-boot -boot-load-size 4 -boot-info-table"
              + " -b boot/grub/i386-pc/eltorito.img -eltorito-alt-boot -e efi.img -no-emul-boot -c boot.catalog -o \""
              + settings->snapshotDir + "/" + filename + "\" . \""
              + settings->workDir + "/iso-2\"";
    } else {
        cmd = "xorriso -as mkisofs -l -V " + volumeLabel
              + " -R -J -pad -iso-level 3 -no-emul-boot -boot-load-size 4 -boot-info-table "
                "-b boot/isolinux/isolinux.bin -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -c "
                "boot/isolinux/isolinux.cat -o \""
              + settings->snapshotDir + "/" + filename + "\" . \"" + settings->workDir + "/iso-2\"";
    }
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
        if (!settings->isArch) {
            shell.run("isohybrid --uefi \"" + settings->snapshotDir + "/" + filename + "\"");
        }
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
    if (settings->isArch) {
        QString archPackage = package;
        if (package == "mx-installer") {
            if (checkInstalled("gazelle-installer")) {
                return true;
            }
            archPackage = "gazelle-installer";
        }
        if (!shell.runAsRoot("pacman -Sy --noconfirm --needed " + archPackage)) {
            emit messageBox(BoxType::critical, tr("Error"), tr("Could not install ") + archPackage);
            return false;
        }
    } else {
        shell.runAsRoot("apt-get update");
        if (!shell.runAsRoot("apt-get install -y " + package)) {
            emit messageBox(BoxType::critical, tr("Error"), tr("Could not install ") + package);
            return false;
        }
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
        if (!shell.run(
                QString("DUF=$(du -BM \"%1\" |grep -oE '^[[:digit:]]+'); TDA=$(df -BM --output=avail /tmp |grep -oE "
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

QString Work::initramfsKernelVersion(const QString &initramfsPath) const
{
    if (!QFileInfo::exists(initramfsPath)) {
        return {};
    }

    QString listCmd;
    const QString lsinitcpio = QStandardPaths::findExecutable("lsinitcpio");
    if (lsinitcpio.isEmpty()) {
        if (QStandardPaths::findExecutable("cpio").isEmpty()) {
            return {};
        }
        listCmd = QString("cpio -it < \"%1\" 2>/dev/null").arg(initramfsPath);
    } else {
        listCmd = QString("\"%1\" -a \"%2\" 2>/dev/null").arg(lsinitcpio, initramfsPath);
    }
    const QString cmd = listCmd
        + " | awk -F/ '{for (i=1; i<=NF; i++) if ($i == \"modules\" && (i+1) <= NF) {print $(i+1); exit}}'";

    return Cmd().getOut(cmd, Cmd::QuietMode::Yes).trimmed();
}

QString Work::kernelImageVersion(const QString &kernelPath) const
{
    if (!QFileInfo::exists(kernelPath)) {
        return {};
    }
    if (QStandardPaths::findExecutable("file").isEmpty()) {
        return {};
    }
    const QString output = Cmd().getOut("file -b \"" + kernelPath + "\"", Cmd::QuietMode::Yes);
    const QRegularExpression versionRegex(R"(version\s+([^\s,]+))");
    const auto match = versionRegex.match(output);
    return match.hasMatch() ? match.captured(1) : QString();
}

bool Work::rebuildArchisoInitramfs(const QString &archisoPath, const QString &kernelPath)
{
    if (QStandardPaths::findExecutable("mkinitcpio").isEmpty()) {
        qWarning() << "mkinitcpio not found; cannot rebuild archiso initramfs.";
        return false;
    }
    if (!QFileInfo::exists(kernelPath)) {
        qWarning() << "Kernel image not found:" << kernelPath;
        return false;
    }

    QString presetName;
    const QDir presetDir("/etc/mkinitcpio.d");
    if (presetDir.exists()) {
        const QStringList presets = presetDir.entryList({"*.preset"}, QDir::Files, QDir::Name);
        for (const QString &presetFile : presets) {
            if (presetFile == "archiso.preset") {
                presetName = "archiso";
                break;
            }
            QFile file(presetDir.filePath(presetFile));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                continue;
            }
            QTextStream stream(&file);
            while (!stream.atEnd()) {
                const QString line = stream.readLine();
                if (line.contains("archiso.img")) {
                    presetName = presetFile.left(presetFile.size() - QString(".preset").size());
                    break;
                }
            }
            if (!presetName.isEmpty()) {
                break;
            }
        }
    }

    QString cmd;
    if (!presetName.isEmpty()) {
        cmd = "mkinitcpio -p " + presetName;
    } else {
        const QStringList configCandidates {
            "/usr/lib/archiso/mkinitcpio.conf",
            "/etc/mkinitcpio-archiso.conf",
            "/usr/share/archiso/configs/releng/airootfs/etc/mkinitcpio.conf.d/archiso.conf",
            "/usr/share/archiso/configs/baseline/airootfs/etc/mkinitcpio.conf.d/archiso.conf",
        };
        QString configPath;
        for (const QString &candidate : configCandidates) {
            if (QFileInfo::exists(candidate)) {
                configPath = candidate;
                break;
            }
        }
        if (configPath.isEmpty()) {
            qWarning() << "No archiso preset or config found for rebuilding archiso initramfs.";
            return false;
        }
        cmd = QString("mkinitcpio -c \"%1\" -k \"%2\" -g \"%3\"").arg(configPath, kernelPath, archisoPath);
    }
    emit message(tr("Rebuilding initramfs with: %1").arg(cmd));
    if (!shell.runAsRoot(cmd)) {
        return false;
    }
    return QFileInfo::exists(archisoPath);
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
    const QString grubCfgPath = settings->workDir + grub_cfg;
    if (QFileInfo::exists(grubCfgPath)) {
        replaceStringInFile("%DISTRO%", settings->projectName + "-" + settings->distroVersion, grubCfgPath);
        replaceStringInFile("%DISTRO_NAME%", settings->projectName, grubCfgPath);
        replaceStringInFile("%FULL_DISTRO_NAME%", settings->fullDistroName, grubCfgPath);
        replaceStringInFile("%FULL_DISTRO_NAME_SPACE%", fullDistroNameSpace, grubCfgPath);
        replaceStringInFile("%RELEASE_DATE%", settings->releaseDate, grubCfgPath);
    }

    const QString grubenv_cfg {"/iso-template/boot/grub/grubenv.cfg"};
    const QString boot_pararameter_regexp {"(lang|kbd|kbvar|kbopt|tz)=[^[:space:]]*"};
    const QString grubenvPath = settings->workDir + grubenv_cfg;
    if (QFileInfo::exists(grubenvPath)) {
        shell.run(QString("printf '%s\\n' %1 | grep -E '^%2' >> '%3'")
                      .arg(settings->bootOptions, boot_pararameter_regexp, grubenvPath));
    }
    if (QFileInfo::exists(grubCfgPath)) {
        shell.run(
            QString(
                R"(sed -i "s|%OPTIONS%|$(sed -r 's/[[:space:]]%2/ /g; s/^[[:space:]]+//; s/[[:space:]]+/ /g'<<<' %1')|" '%3')")
                .arg(settings->bootOptions, boot_pararameter_regexp, grubCfgPath));
    }
    const QString syslinux_cfg {"/iso-template/boot/syslinux/syslinux.cfg"};
    const QString isolinux_cfg {"/iso-template/boot/isolinux/isolinux.cfg"};
    for (const QString &file : {syslinux_cfg, isolinux_cfg}) {
        const QString fullPath = settings->workDir + file;
        if (!QFileInfo::exists(fullPath)) {
            continue;
        }
        replaceStringInFile("%OPTIONS%", settings->bootOptions, fullPath);
        replaceStringInFile("%CODE_NAME%", settings->codename, fullPath);
    }

    const QString sys_readme = "/iso-template/boot/syslinux/readme.msg";
    const QString iso_readme = "/iso-template/boot/isolinux/readme.msg";
    const QStringList cfg_files {syslinux_cfg, isolinux_cfg, sys_readme, iso_readme};
    for (const QString &file : cfg_files) {
        const QString fullPath = settings->workDir + file;
        if (!QFileInfo::exists(fullPath)) {
            continue;
        }
        replaceStringInFile("%FULL_DISTRO_NAME%", settings->fullDistroName, fullPath);
        replaceStringInFile("%RELEASE_DATE%", settings->releaseDate, fullPath);
    }

    QDir themeDir(settings->workDir + "/iso-template/boot/grub/theme");
    if (!themeDir.exists()) {
        return;
    }
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
    if (settings->isArch) {
        const QString archCpuDir = settings->x86 ? "i686" : "x86_64";
        const QString archDir = settings->workDir + "/iso-template/arch";
        QDir().mkpath(archDir);
        const QString fullName = QString("%1/pkglist.%2.txt").arg(archDir, archCpuDir);
        const QString cmd = QString("pacman -Q | awk '{print $1 \" \" $2}' > '%1'").arg(fullName);
        shell.run(cmd);
        return;
    }
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

    if (!setupBindRootOverlay()) {
        emit messageBox(BoxType::critical, tr("Error"),
                        tr("Could not prepare a safe bind-root overlay. Snapshot cannot continue."));
        cleanUp();
    }

    const QString bindWorkDir = settings->workDir + "/bind-root-work";
    BindRootManager bindManager(shell, bindRootPath, bindWorkDir);
    if (!bindManager.start(true)) {
        emit messageBox(BoxType::critical, tr("Error"),
                        tr("Could not prepare the snapshot bind-root environment."));
        cleanUp();
    }

    if (settings->resetAccounts) {
        if (!bind_boot.isEmpty() && !bindManager.doBindMounts({"/boot"})) {
            emit messageBox(BoxType::critical, tr("Error"),
                            tr("Could not prepare the snapshot bind-root environment."));
            cleanUp();
        }
        bool ok = true;
        if (!bindManager.doEmptyDirs({"/home"})) {
            qWarning() << "Bind-root: doEmptyDirs(/home) failed.";
            ok = false;
        }
        if (ok && !bindManager.doGeneral()) {
            qWarning() << "Bind-root: doGeneral failed.";
            ok = false;
        }
        if (ok && !bindManager.doVersionFile()) {
            qWarning() << "Bind-root: doVersionFile failed.";
            ok = false;
        }
        if (!ok) {
            emit messageBox(BoxType::critical, tr("Error"),
                            tr("Could not prepare the snapshot bind-root environment."));
            cleanUp();
        }
    } else {
        QStringList bindDirs {"/home"};
        if (!bind_boot_too.isEmpty()) {
            bindDirs << "/boot";
        }
        bool ok = true;
        if (!bindManager.doBindMounts(bindDirs)) {
            qWarning() << "Bind-root: doBindMounts failed.";
            ok = false;
        }
        if (ok && !bindManager.doLiveFiles()) {
            qWarning() << "Bind-root: doLiveFiles failed.";
            ok = false;
        }
        if (ok && !bindManager.doVersionFile()) {
            qWarning() << "Bind-root: doVersionFile failed.";
            ok = false;
        }
        if (ok && !bindManager.doAdjtime()) {
            qWarning() << "Bind-root: doAdjtime failed.";
            ok = false;
        }
        if (!ok) {
            emit messageBox(BoxType::critical, tr("Error"),
                            tr("Could not prepare the snapshot bind-root environment."));
            cleanUp();
        }
    }
    if (!bindRootOverlayActive) {
        if (!bindManager.makeReadOnly()) {
            qWarning() << "Failed to set bind-root environment read-only.";
        }
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
    const QString elevateTool = Cmd::elevationTool();
    Cmd().run(elevateTool + " " + snapshotLib + " datetime_log", Cmd::QuietMode::Yes);
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
    const QString archCpuDir = settings->x86 ? "i686" : "x86_64";
    const QString infoPath = settings->isArch
        ? settings->workDir + "/iso-template/arch/" + archCpuDir + "/airootfs.info"
        : settings->workDir + "/iso-template/antiX/linuxfs.info";
    QSettings file(infoPath, QSettings::NativeFormat);
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
    QString sizeRoot = bindRootPath;
    if (bindRootOverlayActive) {
        const QString overlayLower = bindRootOverlayBase + "/lower";
        if (QFileInfo::exists(overlayLower)) {
            sizeRoot = overlayLower;
        }
    }
    QString sizeRootPrefix = sizeRoot;
    if (!sizeRootPrefix.endsWith('/')) {
        sizeRootPrefix += "/";
    }
    const QStorageInfo sizeRootInfo(sizeRoot);
    const QByteArray sizeRootDevice = sizeRootInfo.device();
    QStorageInfo rootInfo("/");
    QStorageInfo homeInfo("/home");
    QByteArray rootDevice = sizeRootDevice;
    QByteArray homeDevice;
    bool includeHomeDevice = false;
    if (!settings->live) {
        rootDevice = rootInfo.device();
        if (homeInfo.isValid() && homeInfo.isRoot() && homeInfo.device() != rootDevice) {
            homeDevice = homeInfo.device();
            includeHomeDevice = true;
        }
    }
    const auto isBindMount = [](const QString &mountPoint) -> bool {
        QFile mounts(QStringLiteral("/proc/self/mounts"));
        if (!mounts.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream in(&mounts);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 4) {
                continue;
            }
            QString target = parts.at(1);
            target.replace(QStringLiteral("\\040"), QStringLiteral(" "));
            if (target != mountPoint) {
                continue;
            }
            const QStringList options = parts.at(3).split(',', Qt::SkipEmptyParts);
            return options.contains(QStringLiteral("bind")) || options.contains(QStringLiteral("rbind"));
        }
        return false;
    };
    // If /home is bind-mounted or reset (empty in overlay), exclude it from the size estimate.
    if (!settings->live && homeInfo.isValid() && homeInfo.isRoot() && homeInfo.device() == rootDevice) {
        bool shouldExclude = isBindMount(QStringLiteral("/home"));
        if (!shouldExclude && bindRootOverlayActive) {
            const QString overlayHome = bindRootOverlayBase + "/root/home";
            QDir homeDir(overlayHome);
            if (homeDir.exists() && homeDir.isEmpty()) {
                shouldExclude = true;
            }
        }
        if (shouldExclude) {
            excludes << QStringLiteral("home");
        }
    }
    const auto containsWildcard = [](const QString &path) -> bool {
        const QString wildcards = "*?[{";
        for (const QChar &wildcard : wildcards) {
            if (path.contains(wildcard)) {
                return true;
            }
        }
        return false;
    };
    const auto normalizeExclude = [](QString path) -> QString {
        path.replace(QRegularExpression("/+"), "/");
        if (path.size() > 1 && path.endsWith('/')) {
            path.chop(1);
        }
        return path;
    };
    const QString sizeRootBase = sizeRootPrefix.endsWith('/') ? sizeRootPrefix.left(sizeRootPrefix.size() - 1)
                                                              : sizeRootPrefix;
    const auto isAllowedDevice = [&](const QString &path) -> bool {
        QFileInfo info(path);
        QString probePath = path;
        if (info.isSymLink()) {
            probePath = info.dir().absolutePath();
        }
        const QStorageInfo probeInfo(probePath);
        if (!probeInfo.isValid()) {
            return false;
        }
        const QByteArray probeDevice = probeInfo.device();
        return probeDevice == rootDevice || (includeHomeDevice && probeDevice == homeDevice);
    };
    // Expand patterns without shell globbing or symlinked intermediate traversal.
    const auto expandExcludePattern = [&](QString rawPattern) -> QStringList {
        if (rawPattern.startsWith('/')) {
            rawPattern.remove(0, 1);
        }
        rawPattern.replace(QRegularExpression("/\\*$"), ""); // Remove trailing /*
        QString fullPattern = QDir(sizeRootPrefix).filePath(rawPattern);
        fullPattern = QDir::cleanPath(fullPattern);
        QString relativePattern = fullPattern;
        if (relativePattern.startsWith(sizeRootBase)) {
            relativePattern.remove(0, sizeRootBase.size());
        }
        if (relativePattern.startsWith('/')) {
            relativePattern.remove(0, 1);
        }
        if (relativePattern.isEmpty()) {
            return {sizeRootBase};
        }
        QStringList components = relativePattern.split('/', Qt::SkipEmptyParts);
        QStringList current {sizeRootBase};
        for (int i = 0; i < components.size(); ++i) {
            const QString &component = components.at(i);
            const bool isLast = (i == components.size() - 1);
            QStringList next;
            if (containsWildcard(component)) {
                const QRegularExpression regex(QRegularExpression::wildcardToRegularExpression(component));
                if (!regex.isValid()) {
                    continue;
                }
                for (const QString &base : current) {
                    QFileInfo baseInfo(base);
                    if (!baseInfo.exists() || !baseInfo.isDir() || baseInfo.isSymLink()) {
                        continue;
                    }
                    QDir dir(base);
                    QDir::Filters filters = QDir::NoDotAndDotDot;
                    if (isLast) {
                        filters |= QDir::AllEntries;
                    } else {
                        filters |= QDir::Dirs | QDir::NoSymLinks;
                    }
                    const QFileInfoList entries = dir.entryInfoList(filters);
                    for (const QFileInfo &entry : entries) {
                        if (!regex.match(entry.fileName()).hasMatch()) {
                            continue;
                        }
                        if (!isLast && entry.isSymLink()) {
                            continue;
                        }
                        next.append(entry.filePath());
                    }
                }
            } else {
                for (const QString &base : current) {
                    const QString candidate = QDir(base).filePath(component);
                    QFileInfo candidateInfo(candidate);
                    if (!candidateInfo.exists() && !candidateInfo.isSymLink()) {
                        continue;
                    }
                    if (!isLast) {
                        if (!candidateInfo.isDir() || candidateInfo.isSymLink()) {
                            continue;
                        }
                    }
                    next.append(candidate);
                }
            }
            current = next;
            if (current.isEmpty()) {
                break;
            }
        }
        return current;
    };
    QStringList expandedExcludes;
    expandedExcludes.reserve(excludes.size());
    for (const QString &rawValue : excludes) {
        QString cleaned = rawValue;
        const int bangIndex = cleaned.indexOf('!');
        if (bangIndex != -1) { // Truncate things like "!(minstall.desktop)"
            cleaned.truncate(bangIndex);
        }
        if (cleaned.isEmpty()) {
            continue;
        }
        const QStringList matches = expandExcludePattern(cleaned);
        for (const QString &match : matches) {
            if (!isAllowedDevice(match)) {
                continue;
            }
            const QString normalized = normalizeExclude(match);
            if (!normalized.isEmpty()) {
                expandedExcludes.append(normalized);
            }
        }
    }
    excludes = expandedExcludes;

    // Filter out nested paths to avoid double-counting in size calculation
    std::sort(excludes.begin(), excludes.end(), [](const QString &a, const QString &b) {
        return a.length() < b.length();
    });

    QStringList filteredExcludes;
    for (const QString &path : excludes) {
        bool isNested = false;
        for (const QString &accepted : filteredExcludes) {
            if (accepted == "/") {
                isNested = path != "/";
                break;
            }
            if (path == accepted || path.startsWith(accepted + '/')) {
                isNested = true;
                break;
            }
        }
        if (!isNested) {
            filteredExcludes.append(path);
        }
    }
    excludes = filteredExcludes;

    emit message(tr("Calculating total size of excluded files..."));
    bool ok = true;
    quint64 excl_size = 0;
    if (!excludes.isEmpty()) {
        QTemporaryFile excludeList;
        excludeList.setAutoRemove(true);
        if (!excludeList.open()) {
            ok = false;
        } else {
            for (const QString &path : excludes) {
                excludeList.write(path.toLocal8Bit());
                excludeList.write("\0", 1);
            }
            excludeList.flush();
            const QString cmd = settings->live ? "du -sc -P --apparent-size" : "du -sxc -P --apparent-size";
            const QString cmdLine = cmd + " --files0-from=\"" + excludeList.fileName() + "\""
                                    + " 2>/dev/null | tail -1 | cut -f1";
            excl_size = shell.getOutAsRoot(cmdLine).toULongLong(&ok);
            excludeList.close();
        }
    }
    if (!ok) {
        qDebug() << "Error: calculating size of excluded files\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    emit message(tr("Calculating size of root..."));
    quint64 root_size = 0;
    QString cmd;
    if (settings->live) {
        cmd = "du -s -P --apparent-size";
        root_size = shell.getOutAsRoot(cmd + " \"" + sizeRoot + "\" 2>/dev/null |tail -1 |cut -f1").toULongLong(&ok);
    } else {
        ok = rootInfo.isValid() && rootInfo.isReady();
        if (ok) {
            root_size = (rootInfo.bytesTotal() - rootInfo.bytesFree()) / 1024;
            if (includeHomeDevice) {
                ok = homeInfo.isValid() && homeInfo.isReady();
                if (ok) {
                    root_size += (homeInfo.bytesTotal() - homeInfo.bytesFree()) / 1024;
                }
            }
        }
    }
    constexpr double kibToMib = 1024.0;
    if (!ok) {
        qDebug() << "Error: calculating root size.\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    qDebug().noquote() << "SIZE         " << QString::number(root_size / kibToMib, 'f', 2) << "MiB";
    qDebug().noquote() << "SIZE EXCLUDES" << QString::number(excl_size / kibToMib, 'f', 2) << "MiB";
    const uint c_factor = settings->compressionFactor.value(settings->compression);
    qDebug() << "COMPRESSION  " << c_factor;
    qDebug().noquote() << "SIZE NEEDED  "
                       << QString::number(((root_size - excl_size) * c_factor / 100.0) / kibToMib, 'f', 2) << "MiB";
    qDebug().noquote() << "SIZE FREE    " << QString::number(settings->freeSpace / kibToMib, 'f', 2) << "MiB" << '\n';

    if (excl_size > root_size) {
        qDebug() << "Error: calculating excluded file size.\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    return (root_size - excl_size) * c_factor / 100;
}
