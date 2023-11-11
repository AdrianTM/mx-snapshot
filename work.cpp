/**********************************************************************
 *  work.cpp
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

#include "work.h"

#include <QDate>
#include <QDebug>
#include <QDirIterator>
#include <QRegularExpression>
#include <QSettings>

const extern QFile logFile;

Work::Work(Settings *settings, QObject *parent)
    : QObject(parent),
      settings(settings)
{
}

// Checks if there's enough space on partitions, if not post error, cleanup and exit
// We don't yet take /home used space into considerations (need to calculate how much is excluded)
void Work::checkEnoughSpace()
{
    quint64 required_space = getRequiredSpace();
    // Check foremost if enough space for ISO on snapshot_dir, print error and exit if not
    checkNoSpaceAndExit(required_space, settings->free_space, settings->snapshot_dir);

    /* If snapshot and workdir are on the same partition we need about double the size of required_space.
     * If both TMP work_dir and ISO don't fit on snapshot_dir, see if work_dir can be put on /home or /tmp
     * we already checked that ISO can fit on snapshot_dir so if TMP work fits on /home or /tmp move
     * the work_dir to the appropriate place and return */
    if (Cmd().getOut("stat -c '%d' " + settings->work_dir) == Cmd().getOut("stat -c '%d' " + settings->snapshot_dir)) {
        if (settings->free_space < required_space * 2) {
            if (checkAndMoveWorkDir("/tmp", required_space)) {
                return;
            }
            if (checkAndMoveWorkDir("/home", required_space)) {
                return;
            }
            checkNoSpaceAndExit(required_space * 2, settings->free_space,
                                settings->snapshot_dir); // Print out error and exit
        }
    } else { // If not on the same partitions, check if work_dir has enough free space for temp files for required_space
        checkNoSpaceAndExit(required_space, settings->free_space_work, settings->work_dir);
    }
}

// Checks if package is installed
bool Work::checkInstalled(const QString &package)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    return (Cmd().run(QString("dpkg -s %1 |grep '^Status: install ok installed'").arg(package)));
}

// Clean up changes before exit
void Work::cleanUp()
{
    if (!started) {
        shell.close();
        initrd_dir.remove();
        exit(EXIT_SUCCESS);
    }
    shell.close();
    emit message(tr("Cleaning..."));
    Cmd().run("sync");

    Cmd().runAsRoot("pkill mksquashfs", true);
    Cmd().runAsRoot("pkill md5sum", true);
    QDir::setCurrent("/");
    if (QFileInfo::exists("/tmp/installed-to-live/cleanup.conf")) {
        Cmd().runAsRoot("installed-to-live cleanup");
    }

    if (!settings->live && !settings->reset_accounts) {
        auto homeDirs = QDir("/home").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &user : homeDirs) {
            QString desktopPath = QString("/home/%1/Desktop/minstall.desktop").arg(user);
            QFile::remove(desktopPath);
        }
    }

    Cmd().runAsRoot(
        "rm /usr/local/share/live-files/files/etc/mx-version /usr/local/share/live-files/files/etc/lsb-release", true);

    if (!settings->live) {
        Cmd().runAsRoot("rm /etc/skel/Desktop/Installer.desktop", true);
    }

    initrd_dir.remove();
    settings->tmpdir.reset();
    if (done) {
        qDebug().noquote() << tr("Done");
        if (settings->shutdown) {
            QFile::copy(logFile.fileName(), settings->snapshot_dir + "/" + settings->snapshot_name + ".log");
            Cmd().run("sync");
            Cmd().runAsRoot("shutdown -h now &");
        }
        exit(EXIT_SUCCESS);
    } else {
        qDebug().noquote() << tr("Done") << "\n";
        qDebug().noquote() << QObject::tr("Interrupted or failed to complete");
        exit(EXIT_FAILURE);
    }
}

// Check if we can put work_dir on another partition with enough space, move work_dir there and setupEnv again
bool Work::checkAndMoveWorkDir(const QString &dir, quint64 req_size)
{
    // See first if the dir is on different partition otherwise it's irrelevant
    if (Cmd().getOut("stat -c '%d' " + dir) != Cmd().getOut("stat -c '%d' " + settings->snapshot_dir)
        && Settings::getFreeSpace(dir) > req_size) {
        if (QFileInfo::exists("/tmp/installed-to-live/cleanup.conf")) {
            shell.runAsRoot("installed-to-live cleanup");
        }
        settings->tempdir_parent = dir;
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
                    .arg(QString::number(needed_space / factor, 'f', 2) + "GiB")
                + "\n"
                + tr("You have %1 free space on %2").arg(QString::number(free_space / factor, 'f', 2) + "GiB", dir)
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
    makeChecksum(HashType::md5, settings->work_dir + "/iso-template/antiX", "initrd.gz");
}

// copyModules(mod_dir/kernel kernel)
void Work::copyModules(const QString &to, const QString &kernel)
{
    shell.run(QString(R"(/usr/share/%1/scripts/copy-initrd-modules -t="%2" -k="%3")")
                  .arg(qApp->applicationName(), to, kernel));
    shell.run(QString("/usr/share/%1/scripts/copy-initrd-programs --to=\"%2\"").arg(qApp->applicationName(), to));
}

// Copying the iso-template filesystem
void Work::copyNewIso()
{
    emit message(tr("Copying the new-iso filesystem..."));
    QDir::setCurrent(settings->work_dir);

    shell.run("tar xf /usr/lib/iso-template/iso-template.tar.gz");
    shell.run("cp /usr/lib/iso-template/template-initrd.gz iso-template/antiX/initrd.gz");
    shell.run("cp /boot/vmlinuz-" + settings->kernel + " iso-template/antiX/vmlinuz");

    replaceMenuStrings();
    makeChecksum(HashType::md5, settings->work_dir + "/iso-template/antiX", "vmlinuz");

    QString path = initrd_dir.path();
    if (!initrd_dir.isValid()) {
        qDebug() << tr("Could not create temp directory. ") + path;
        cleanUp();
    }

    openInitrd(settings->work_dir + "/iso-template/antiX/initrd.gz", path);

    // Strip modules; make sure initrd_dir is correct to avoid disaster
    if (path.startsWith("/tmp/")) {
        QDir modulesDir(path + "/lib/modules");
        if (modulesDir.exists()) {
            modulesDir.removeRecursively();
        }
    }

    // For old versions we copy initrd-release for newere ones we copy initrd_release
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
        closeInitrd(path, settings->work_dir + "/iso-template/antiX/initrd.gz");
        initrd_dir.remove();
    }
}

// Create squashfs and then the iso
bool Work::createIso(const QString &filename)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Squash the filesystem copy
    QDir::setCurrent(settings->work_dir);
    QString maybe_unbuffer = (settings->cli_mode && checkInstalled("expect")) ? "unbuffer " : "";
    QString cmd = maybe_unbuffer + "mksquashfs /.bind-root " + settings->work_dir + "/iso-template/antiX/linuxfs -comp "
                  + settings->compression + ((settings->mksq_opt.isEmpty()) ? "" : " " + settings->mksq_opt)
                  + " -wildcards -ef " + settings->snapshot_excludes.fileName() + " " + settings->session_excludes;

    emit message(tr("Squashing filesystem..."));
    if (!shell.runAsRoot(cmd)) {
        emit messageBox(BoxType::critical, tr("Error"),
                        tr("Could not create linuxfs file, please check whether you have enough space on the "
                           "destination partition."));
        return false;
    }
    writeUnsquashfsSize(shell.readAll());

    // Move linuxfs files to iso-2/antiX folder
    QDir().mkpath("iso-2/antiX");
    shell.run("mv iso-template/antiX/linuxfs* iso-2/antiX");
    makeChecksum(HashType::md5, settings->work_dir + "/iso-2/antiX", "linuxfs");

    shell.runAsRoot("installed-to-live cleanup");

    // Create the iso file
    QDir::setCurrent(settings->work_dir + "/iso-template");
    cmd = "xorriso -as mkisofs -l -V MXLIVE -R -J -pad -iso-level 3 -no-emul-boot -boot-load-size 4 -boot-info-table "
          "-b boot/isolinux/isolinux.bin -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -c "
          "boot/isolinux/isolinux.cat -o \""
          + settings->snapshot_dir + "/" + filename + "\" . \"" + settings->work_dir + "/iso-2\"";
    emit message(tr("Creating CD/DVD image file..."));
    if (!shell.run(cmd)) {
        emit messageBox(
            BoxType::critical, tr("Error"),
            tr("Could not create ISO file, please check whether you have enough space on the destination partition."));
        return false;
    }

    // Make it isohybrid
    if (settings->make_isohybrid) {
        emit message(tr("Making hybrid iso"));
        shell.run("isohybrid --uefi \"" + settings->snapshot_dir + "/" + filename + "\"");
    }

    // Make ISO checksums
    if (settings->make_md5sum) {
        makeChecksum(HashType::md5, settings->snapshot_dir, filename);
    }
    if (settings->make_sha512sum) {
        makeChecksum(HashType::sha512, settings->snapshot_dir, filename);
    }
    shell.runAsRoot("chown $(logname):$(logname) \"" + settings->snapshot_dir + "/" + filename + "\"*");

    QTime time(0, 0);
    time = time.addMSecs(e_timer.elapsed());
    emit message(tr("Done"));
    if (settings->shutdown) {
        done = true;
        cleanUp();
    }
    emit messageBox(BoxType::information, tr("Success"),
                    tr("MX Snapshot completed sucessfully!") + "\n"
                        + tr("Snapshot took %1 to finish.").arg(time.toString("hh:mm:ss")) + "\n\n"
                        + tr("Thanks for using MX Snapshot, run MX Live USB Maker next!"));
    done = true;
    return true;
}

// Installs package
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
    QString checksum_cmd = ("%1sum \"" + file_name + "\">\"" + folder + "/" + file_name + ".%1\"").arg(ce);
    QString temp_dir {"/tmp/snapsphot-checksum-temp"};
    QString checksum_tmp
        = ("TD=" + temp_dir + "; KEEP=$TD/.keep; [ -d $TD ] || mkdir $TD ; FN=\"" + file_name + "\"; CF=\"" + folder
           + "/${FN}.%1\"; cp $FN $TD/$FN; pushd $TD>/dev/null; %1sum $FN > $FN.%1 ; cp $FN.%1 "
             "$CF; popd >/dev/null ; [ -e $KEEP ] || rm -rf $TD")
              .arg(ce);

    if (settings->preempt) {
        // Check free space available on /tmp
        shell.run("TF=/tmp/snapsphot-checksum-temp/\"" + file_name + R"("; [ -f "$TF" ] && rm -f "$TF")");
        if (!shell.run(
                "DUF=$(du -BM " + file_name
                + "|grep -oE '^[[:digit:]]+'); TDA=$(df -BM --output=avail /tmp |grep -oE '^[[:digit:]]+'); ((TDA/10*8 "
                  ">= DUF))")) {
            settings->preempt = false;
        }
    }
    if (!settings->preempt) {
        cmd = checksum_cmd;
    } else {
        // Free pagecache
        shell.runAsRoot("sync; sleep 1; echo 1 > /proc/sys/vm/drop_caches; sleep 1");
        cmd = checksum_tmp;
    }
    shell.run(cmd);
    QDir::setCurrent(settings->work_dir);
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
    QString full_distro_name_space = settings->full_distro_name;
    full_distro_name_space.replace("_", " ");

    const QString grub_cfg {"/iso-template/boot/grub/grub.cfg"};
    replaceStringInFile("%DISTRO%", settings->project_name + "-" + settings->distro_version,
                        settings->work_dir + grub_cfg);
    replaceStringInFile("%DISTRO_NAME%", settings->project_name, settings->work_dir + grub_cfg);
    replaceStringInFile("%FULL_DISTRO_NAME%", settings->full_distro_name, settings->work_dir + grub_cfg);
    replaceStringInFile("%FULL_DISTRO_NAME_SPACE%", full_distro_name_space, settings->work_dir + grub_cfg);
    replaceStringInFile("%RELEASE_DATE%", settings->release_date, settings->work_dir + grub_cfg);

    const QString grubenv_cfg {"/iso-template/boot/grub/grubenv.cfg"};
    const QString boot_pararameter_regexp {"^(lang=|kbd=|kbvar=|kbopt=|tz=)"};
    shell.run(QString("printf '%s\\n' %1 | grep -E '%2' >> '%3'")
                  .arg(settings->boot_options, boot_pararameter_regexp, settings->work_dir + grubenv_cfg));
    shell.run(QString(R"(sed -i "s|%OPTIONS%|$(printf '%s\n' %1 | grep -v -E '%2' | tr '\n' ' ')|" '%3')")
                  .arg(settings->boot_options, boot_pararameter_regexp, settings->work_dir + grub_cfg));

    const QString syslinux_cfg {"/iso-template/boot/syslinux/syslinux.cfg"};
    const QString isolinux_cfg {"/iso-template/boot/isolinux/isolinux.cfg"};
    for (const QString &file : {syslinux_cfg, isolinux_cfg}) {
        replaceStringInFile("%OPTIONS%", settings->boot_options, settings->work_dir + file);
        replaceStringInFile("%CODE_NAME%", settings->codename, settings->work_dir + file);
    }

    const QString sys_readme = "/iso-template/boot/syslinux/readme.msg";
    const QString iso_readme = "/iso-template/boot/isolinux/readme.msg";
    const QStringList cfg_files {syslinux_cfg, isolinux_cfg, sys_readme, iso_readme};
    for (const QString &file : cfg_files) {
        replaceStringInFile("%FULL_DISTRO_NAME%", settings->full_distro_name, settings->work_dir + file);
        replaceStringInFile("%RELEASE_DATE%", settings->release_date, settings->work_dir + file);
    }

    QString themeDir = settings->work_dir + "/iso-template/boot/grub/theme";
    QDirIterator themeFileIt(themeDir, {"*.txt"}, QDir::Files);
    while (themeFileIt.hasNext()) {
        QString themeFile = themeFileIt.next();
        replaceStringInFile("%ASCII_CODE_NAME%", settings->codename, themeFile);
        replaceStringInFile("%DISTRO%", settings->project_name + "-" + settings->distro_version, themeFile);
    }
}

// Util function for replacing strings in files
bool Work::replaceStringInFile(const QString &old_text, const QString &new_text, const QString &file_path)
{
    qDebug() << "REPLACE STRIGN IN FILE" << file_path;
    qDebug() << "OLD" << old_text << "NEW" << new_text;
    qDebug() << "CURRENT PATH" << QDir().currentPath();
    bool result = shell.runAsRoot(QString("sed -i 's|%1|%2|g' %3").arg(old_text, new_text, file_path));
    qDebug() << "RESULT" << result;
    return result;
}

// Save package list in working directory
void Work::savePackageList(const QString &file_name)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QFileInfo fi(file_name);
    QDir dir(settings->work_dir + "/iso-template/" + fi.completeBaseName());
    if (!dir.mkpath(dir.absolutePath())) {
        emit messageBox(BoxType::critical, tr("Error"),
                        tr("Could not create working directory. ") + dir.absolutePath());
    }
    QString full_name = settings->work_dir + "/iso-template/" + fi.completeBaseName() + "/package_list";
    QString cmd
        = R"(dpkg -l |grep ^ii\ \ |awk '{print $2,$3}' |sed 's/:'$(dpkg --print-architecture)'//' |column -t >")"
          + full_name + "\"";
    shell.run(cmd);
}

// Setup the environment before taking the snapshot
void Work::setupEnv()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    // Checks if work_dir looks OK
    if (!settings->work_dir.contains("/mx-snapshot")) {
        cleanUp();
    }

    QString bind_boot;
    QString bind_boot_too;
    if (shell.run("mountpoint /boot")) {
        bind_boot = "bind=/boot ";
        bind_boot_too = ",/boot";
    }

    // Install mx-installer if absent
    if (settings->force_installer && !checkInstalled("mx-installer")) {
        installPackage("mx-installer");
    }

    writeSnapshotInfo();
    writeVersionFile();
    writeLsbRelease();

    // Setup environment if creating a respin (reset root/demo, remove personal accounts)
    if (settings->reset_accounts) {
        shell.runAsRoot("installed-to-live -b /.bind-root start " + bind_boot
                        + "empty=/home general version-file read-only");
    } else {
        //        if (settings->force_installer) { // copy minstall.desktop to Desktop on all accounts
        //            shell.runAsRoot("echo /home/*/Desktop |xargs -n1 cp
        //            /usr/share/applications/minstall.desktop 2>/dev/null"); shell.runAsRoot("echo
        //            /home/*/Desktop/minstall.desktop |xargs -n1 sed -i 's/^NoDisplay=true/NoDisplay=false/'");
        //            // Needs write access to remove lock symbol on installer on desktop, executable to run it
        //            shell.runAsRoot("chmod 777 /home/*/Desktop/minstall.desktop");
        //            if (!QFile::exists("/usr/bin/xdg-user-dirs-update.real")) {
        //                QDir().mkdir("/etc/skel/Desktop");
        //                QFile::copy("/usr/share/applications/minstall.desktop",
        //                "/etc/skel/Desktop/Installer.desktop"); RUN("chmod 755 /etc/skel/Desktop/Installer.desktop");
        //            }
        //        }
        shell.runAsRoot("installed-to-live -b /.bind-root start bind=/home" + bind_boot_too
                        + " live-files version-file adjtime read-only");
    }
}

void Work::writeLsbRelease()
{
    QFile file("/usr/local/share/live-files/files/etc/lsb-release");
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream stream(&file);
    stream << "PRETTY_NAME=\"" << settings->project_name << " " << settings->distro_version << " " << settings->codename
           << "\"\n";
    stream << "DISTRIB_ID=\"" << settings->project_name << "\"\n";
    stream << "DISTRIB_RELEASE=" << settings->distro_version << "\n";
    stream << "DISTRIB_CODENAME=\"" << settings->codename << "\"\n";
    stream << "DISTRIB_DESCRIPTION=\"" << settings->project_name << " " << settings->distro_version << " "
           << settings->codename << "\"\n";
    file.close();
}

// Write date of the snapshot in a "snapshot_created" file
void Work::writeSnapshotInfo()
{
    QFile file("/usr/local/share/live-files/files/etc/snapshot_created");
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString("yyyyMMdd_HHmm");
    file.close();
}

void Work::writeVersionFile()
{
    QFile file("/usr/local/share/live-files/files/etc/mx-version");
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream stream(&file);
    stream << settings->full_distro_name << " " << settings->codename << " " << settings->release_date << "\n";
    file.close();
}

void Work::writeUnsquashfsSize(const QString &text)
{
    QSettings file(settings->work_dir + "/iso-template/antiX/linuxfs.info", QSettings::NativeFormat);
    file.setValue("UncompressedSizeKB",
                  text.section(QRegularExpression(" uncompressed filesystem size \\("), 1, 1).section(" ", 0, 0));
}

quint64 Work::getRequiredSpace()
{
    QStringList excludes;
    QFile *file = &settings->snapshot_excludes;

    // Open and read the excludes file
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Count not open file: " << file->fileName();
    }
    while (!file->atEnd()) {
        QString line = file->readLine().trimmed();
        if (!line.startsWith(QLatin1String("#")) && !line.isEmpty() && !line.startsWith(QLatin1String(".bind-root"))) {
            excludes << line.trimmed();
        }
    }
    file->close();

    // Add session excludes
    if (!settings->session_excludes.isEmpty()) {
        QString sessionExcludes = settings->session_excludes;
        sessionExcludes.remove(0, 3); // Remove "-e "

        QStringList excludeList = sessionExcludes.split("\" \"");
        excludes.reserve(excludeList.size());
        for (QString exclude : excludeList) {
            exclude = exclude.replace("\"", "").trimmed();
            excludes << exclude;
        }
    }

    QString root_dev = Cmd().getOut("df /.bind-root --output=target |tail -1", true);
    QMutableStringListIterator it(excludes);
    while (it.hasNext()) {
        it.next();
        if (it.value().indexOf(QLatin1String("!")) != -1) { // remove things like "!(minstall.desktop)"
            it.value().truncate(it.value().indexOf(QLatin1String("!")));
        }
        it.value().replace(QLatin1String(" "),
                           QLatin1String("\\ ")); // escape special bash characters, might need to expand this
        it.value().replace(QLatin1String("("), QLatin1String("\\("));
        it.value().replace(QLatin1String(")"), QLatin1String("\\)"));
        it.value().replace(QLatin1String("|"), QLatin1String("\\|"));
        it.value().prepend("/.bind-root/");            // check size occupied by excluded files on /.bind-root only
        it.value().remove(QRegularExpression("\\*$")); // chop last *
        // Remove from list if files not on the same volume
        if (root_dev != Cmd().getOut("df " + it.value() + " --output=target 2>/dev/null |tail -1", true)) {
            it.remove();
        }
    }
    emit message(tr("Calculating total size of excluded files..."));
    bool ok = false;
    QString cmd = settings->live ? "du -sc" : "du -sxc";
    quint64 excl_size
        = shell.getOut(cmd + " {" + excludes.join(",").remove("/.bind-root,") + "} 2>/dev/null |tail -1 |cut -f1")
              .toULongLong(&ok);
    if (!ok) {
        qDebug() << "Error: calculating size of excluded files\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    emit message(tr("Calculating size of root..."));
    cmd = settings->live ? "du -s" : "du -sx";
    quint64 root_size = shell.getOut(cmd + " /.bind-root 2>/dev/null |tail -1 |cut -f1").toULongLong(&ok);
    if (!ok) {
        qDebug() << "Error: calculating root size (/.bind-root)\n"
                    "If you are sure you have enough free space rerun the program with -o/--override-size option";
        cleanUp();
    }
    qDebug() << "SIZE ROOT    " << root_size;
    qDebug() << "SIZE EXCLUDES" << excl_size;
    uint c_factor = compression_factor.value(settings->compression);
    qDebug() << "COMPRESSION  " << c_factor;
    qDebug() << "SIZE NEEDED  " << (root_size - excl_size) * c_factor / 100;
    qDebug() << "SIZE FREE    " << settings->free_space << "\n";

    if (excl_size > root_size) {
        qDebug() << "Error: calculating excluded file size.\n"
                    "If you are sure you have enough free space rerun the program with -o/--overrde-size option";
        cleanUp();
    }
    return (root_size - excl_size) * c_factor / 100;
}
