/**********************************************************************
 *  mxsnapshot.cpp
 **********************************************************************
 * Copyright (C) 2015 MX Authors
 *
 * Authors: Adrian
 *          MX & MEPIS Community <http://forum.mepiscommunity.org>
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


#include "mxsnapshot.h"
#include "ui_mxsnapshot.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>

//#include <QDebug>

mxsnapshot::mxsnapshot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxsnapshot)
{
    ui->setupUi(this);

    proc = new QProcess(this);
    timer = new QTimer(this);
    proc->setReadChannel(QProcess::StandardOutput);
    proc->setReadChannelMode(QProcess::MergedChannels);

    this->setWindowTitle(tr("MX Snapshot"));
    ui->buttonBack->setHidden(true);
    ui->buttonSelectSnapshot->setHidden(true);
    ui->stackedWidget->setCurrentIndex(0);

    version = getVersion("mx-snapshot");
    live = isLive();
    i686 = isi686();
    setup();
    reset_accounts = false;
    listUsedSpace();
}

mxsnapshot::~mxsnapshot()
{
    delete ui;
}

// load settings or use the default value
void mxsnapshot::loadSettings()
{
    config_file.setFileName("/etc/mx-snapshot.conf");
    QSettings settings(config_file.fileName(), QSettings::IniFormat);

    session_excludes = "";
    snapshot_dir = settings.value("snapshot_dir", "/home/snapshot").toString();
    ui->labelSnapshot->setText(tr("The snapshot will be placed by default in ") + snapshot_dir.absolutePath());
    snapshot_excludes.setFileName(settings.value("snapshot_excludes", "/usr/local/share/excludes/mx-snapshot-exclude.list").toString());
    snapshot_basename = settings.value("snapshot_basename", "snapshot").toString();
    make_md5sum = settings.value("make_md5sum", "no").toString();
    make_isohybrid = settings.value("make_isohybrid", "yes").toString();
    mksq_opt = settings.value("mksq_opt", "-comp xz").toString();
    edit_boot_menu = settings.value("edit_boot_menu", "no").toString();
    lib_mod_dir = settings.value("lib_mod_dir", "/lib/modules/").toString();
    gui_editor.setFileName(settings.value("gui_editor", "/usr/bin/leafpad").toString());
    stamp = settings.value("stamp", "datetime").toString();
}

// setup/refresh versious items first time program runs
void mxsnapshot::setup()
{
    qApp->processEvents();
    this->show();
    this->setWindowTitle(tr("MX Snapshot"));
    ui->buttonBack->setHidden(true);
    ui->buttonSelectSnapshot->setHidden(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonCancel->setEnabled(true);
    ui->buttonNext->setEnabled(true);

    loadSettings();
    listFreeSpace();
}

// Util function for getting bash command output
QString mxsnapshot::getCmdOut(QString cmd)
{
    qApp->processEvents();
    QEventLoop loop;
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    disconnectAll();
    return proc->readAllStandardOutput().trimmed();
}

// Util function for running bash commands with progress bar and output in output box
int mxsnapshot::runCmd(QString cmd)
{
    qApp->processEvents();
    QEventLoop loop;
    setConnections();
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    disconnectAll();
    return proc->exitCode();
}

// Util function for replacing strings in files
bool mxsnapshot::replaceStringInFile(QString oldtext, QString newtext, QString filepath)
{
    QString cmd = QString("sed -i 's/%1/%2/g' %3").arg(oldtext).arg(newtext).arg(filepath);
    if (system(cmd.toUtf8()) != 0) {
        return false;
    }
    return true;
}

// Check if running from a live envoronment
bool mxsnapshot::isLive()
{
    return (system("mountpoint -q /live/aufs") == 0 );
}

// Check if running from a 32bit environment
bool mxsnapshot::isi686()
{
    return (getCmdOut("uname -m") == "i686");
}

// Get version of the program
QString mxsnapshot::getVersion(QString name)
{
    QString cmd = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return getCmdOut(cmd);
}

// return number of snapshots in snapshot_dir
int mxsnapshot::getSnapshotCount()
{
    if (snapshot_dir.exists()) {
        QFileInfoList list = snapshot_dir.entryInfoList(QStringList("*.iso"), QDir::Files);
        return list.size();
    }
    return 0;
}

// return the size of the work folder
QString mxsnapshot::getSnapshotSize()
{
    QString size;
    if (snapshot_dir.exists()) {
        QString cmd = QString("find %1 -maxdepth 1 -type f -name '*.iso' -exec du -shc {} + | tail -1 | awk '{print $1}'").arg(snapshot_dir.absolutePath());
        size = getCmdOut(cmd);
        if (size != "" ) {
            return size;
        }
    }
    return "0";
}

// List used space
void mxsnapshot::listUsedSpace()
{
    this->show();
    ui->buttonNext->setDisabled(true);
    ui->buttonCancel->setDisabled(true);
    ui->buttonSelectSnapshot->setDisabled(true);
    QString cmd;
    if (live) {
        cmd = QString("du --exclude-from=%1 -sch / 2>/dev/null | tail -n1 | cut -f1").arg(snapshot_excludes.fileName());
    } else {
        cmd = QString("df -h / | awk 'NR==2 {print $3}'");
    }
    connect(timer, SIGNAL(timeout()), SLOT(procTime()));
    connect(proc, SIGNAL(started()), SLOT(procStart()));
    connect(proc, SIGNAL(finished(int)), SLOT(procDone(int)));
    QString out = "\n- " + tr("Used space on / (root): ") + getCmdOut(cmd);
    if (system("mountpoint -q /home") == 0 ) {
        cmd = QString("df -h /home | awk 'NR==2 {print $3}'");
        out.append("\n- " + tr("Used space on /home: ") + getCmdOut(cmd));
    }
    ui->buttonNext->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
    ui->buttonSelectSnapshot->setEnabled(true);
    ui->labelUsedSpace->setText(out);
}


// List free space on drives
void mxsnapshot::listFreeSpace()
{
    QString cmd;
    QString out;
    QString path = snapshot_dir.absolutePath().remove("/snapshot");
    cmd = QString("df -h %1 | awk 'NR==2 {print $4}'").arg(path);
    ui->labelFreeSpace->clear();
    out.append("- " + tr("Free space on %1, where snapshot folder is placed: ").arg(path) + getCmdOut(cmd) + "\n");
    ui->labelFreeSpace->setText(out);
    ui->labelDiskSpaceHelp->setText(tr("The free space should be sufficient to hold the compressed data from / and /home\n\n"
                                       "      If necessary, you can create more available space\n"
                                       "      by removing previous snapshots and saved copies:\n"
                                       "      %1 snapshots are taking up %2 of disk space.\n").arg(QString::number(getSnapshotCount())).arg(getSnapshotSize()));
}

// Checks if the editor listed in the config file is present
void mxsnapshot::checkEditor()
{
    if (gui_editor.exists()) {
        return;
    }
    QString msg = tr("The graphical text editor is set to %1, but it is not installed. Edit %2 "
                     "and set the gui_editor variable to the editor of your choice. "
                     "(examples: /usr/bin/gedit, /usr/bin/leafpad)\n\n"
                     "Will install leafpad and use it this time.").arg(gui_editor.fileName()).arg(config_file.fileName());
    QMessageBox::information(0, QString::null, msg);
    if (installPackage("leafpad")) {
        gui_editor.setFileName("/usr/bin/leafpad");
    }
}

// Checks if package is installed
bool mxsnapshot::checkInstalled(QString package)
{
    QString cmd = QString("dpkg -s %1 | grep Status").arg(package);
    if (getCmdOut(cmd) == "Status: install ok installed") {
        return true;
    }
    return false;
}

// Installs package
bool mxsnapshot::installPackage(QString package)
{
    QEventLoop loop;
    this->setWindowTitle(tr("Installing ") + package);
    ui->outputBox->clear();
    ui->buttonNext->setDisabled(true);
    ui->buttonBack->setDisabled(true);
    this->show();
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    setConnections();
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("apt-get update");
    loop.exec();
    proc->start("apt-get install " + package);
    loop.exec();
    disconnectAll();
    this->hide();
    if (proc->exitCode() != 0) {
        QMessageBox::critical(0, tr("Error"), tr("Could not install ") + package);
        return false;
    }
    this->show();
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
    ui->buttonNext->setEnabled(true);
    ui->buttonBack->setEnabled(true);
    return true;
}

void mxsnapshot::checkDirectories()
{
    //  Create snapshot dir if it doesn't exist
    if (!snapshot_dir.exists()) {
        snapshot_dir.mkpath(snapshot_dir.absolutePath());
        QString path = snapshot_dir.absolutePath();

        QString cmd = QString("chmod 777 %1").arg(path);
        system(cmd.toUtf8());
    }

    // Create a work_dir
    work_dir = getCmdOut("mktemp -d " + snapshot_dir.absolutePath() + "/mx-snapshot-XXXXXXXX");
}

void mxsnapshot::openInitrd(QString file, QString initrd_dir)
{
    QString cmd = "mkdir -p " + initrd_dir;
    system(cmd.toUtf8());
    cmd = "chmod a+rx " + initrd_dir;
    system(cmd.toUtf8());
    QDir::setCurrent(initrd_dir);
    cmd = QString("gunzip -c %1 | cpio -idum").arg(file);
    system(cmd.toUtf8());
}

void mxsnapshot::closeInitrd(QString initrd_dir, QString file)
{
    QDir::setCurrent(initrd_dir);
    QString cmd = "(find . | cpio -o -H newc --owner root:root | gzip -9) >" + file;
    system(cmd.toUtf8());
    makeMd5sum(work_dir + "/iso-template/antiX", "initrd.gz");
}

// Copying the iso-template filesystem
void mxsnapshot::copyNewIso()
{
    ui->outputBox->clear();

    ui->outputLabel->setText(tr("Copying the new-iso filesystem..."));
    QDir::setCurrent(work_dir);
    QString cmd = "tar xf /usr/lib/mx-snapshot/iso-template.tar.gz";
    runCmd(cmd);

    cmd = "cp /boot/vmlinuz-" + kernel_used + " " + work_dir + "/iso-template/antiX/vmlinuz";        
    runCmd(cmd);

    if(i686) {
        cmd = "cp /boot/vmlinuz-3.16.0-4-586 " + work_dir + "/iso-template/antiX/vmlinuz1";
        runCmd(cmd);
        // remove x64 template files
        runCmd("rm " + work_dir + "/iso-template/boot/grub/grub.cfg_x64");
        runCmd("rm " + work_dir + "/iso-template/boot/syslinux/syslinux.cfg_x64");
        runCmd("rm " + work_dir + "/iso-template/boot/isolinux/isolinux.cfg_x64");
    } else {
        // mv x64 template files over
        runCmd("mv " + work_dir + "/iso-template/boot/grub/grub.cfg_x64 " + work_dir + "/iso-template/boot/grub/grub.cfg");
        runCmd("mv " + work_dir + "/iso-template/boot/syslinux/syslinux.cfg_x64 " + work_dir + "/iso-template/boot/syslinux/syslinux.cfg");
        runCmd("mv " + work_dir + "/iso-template/boot/isolinux/isolinux.cfg_x64 " + work_dir + "/iso-template/boot/isolinux/isolinux.cfg");
    }
    replaceMenuStrings();

    makeMd5sum(work_dir + "/iso-template/antiX", "vmlinuz");

    QString initrd_dir = work_dir + "/initrd";
    openInitrd(work_dir + "/iso-template/antiX/initrd.gz", initrd_dir);

    QString mod_dir = initrd_dir + "/lib/modules";
    if (initrd_dir != "") {
        copyModules(initrd_dir, kernel_used);
        closeInitrd(initrd_dir, work_dir + "/iso-template/antiX/initrd.gz");
        if (i686) {
            cmd = "cp " + work_dir + "/iso-template/antiX/initrd.gz" + " " + work_dir + "/iso-template/antiX/initrd1.gz";
            system(cmd.toUtf8());
        }
    }
}

// replace text in menu items in grub.cfg, syslinux.cfg, isolinux.cfg
void mxsnapshot::replaceMenuStrings() {
    if (i686) {
        QString new_string = "MX-15_686-pae (" + getCmdOut("date +'%d %B %Y'") + ")";
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/grub/grub.cfg");
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/syslinux/syslinux.cfg");
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/isolinux/isolinux.cfg");
        new_string = "MX-15_586-non-pae (" + getCmdOut("date +'%d %B %Y'") + ")";
        replaceStringInFile("custom-name-non-pae", new_string, work_dir + "/iso-template/boot/grub/grub.cfg");
        replaceStringInFile("custom-name-non-pae", new_string, work_dir + "/iso-template/boot/syslinux/syslinux.cfg");
        replaceStringInFile("custom-name-non-pae", new_string, work_dir + "/iso-template/boot/isolinux/isolinux.cfg");
    } else {
        QString new_string = "MX-15_x64 (" + getCmdOut("date +'%d %B %Y'") + ")";
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/grub/grub.cfg");
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/syslinux/syslinux.cfg");
        replaceStringInFile("custom-name-pae", new_string, work_dir + "/iso-template/boot/isolinux/isolinux.cfg");
    }
}

// copyModules(mod_dir/kernel_used kernel_used)
void mxsnapshot::copyModules(QString to, QString kernel)
{
    QString cmd = QString("copy-initrd-modules -t=%1 -k=%2").arg(to).arg(kernel);
    system(cmd.toUtf8());
}

// Create the output filename
QString mxsnapshot::getFilename()
{
    if (stamp == "datetime") {
        return snapshot_basename + "-" + getCmdOut("date +%Y%m%d_%H%M") + ".iso";
    } else {
        int n = 1;
        QString name = snapshot_dir.absolutePath() + "/" + snapshot_basename + QString::number(n) + ".iso";
        QDir dir;
        dir.setPath(name);
        while (dir.exists(dir.absolutePath())) {
            n++;
            QString name = snapshot_dir.absolutePath() + "/" + snapshot_basename + QString::number(n) + ".iso";
            dir.setPath(name);
        }
        return dir.absolutePath();
    }
}

// make working directory using the base filename
void mxsnapshot::mkDir(QString filename)
{
    QDir dir;
    filename.chop(4); //remove ".iso" string
    dir.setPath(work_dir + "/iso-template/" + filename);
    dir.mkpath(dir.absolutePath());
}

// save package list in working directory
void mxsnapshot::savePackageList(QString filename)
{
    filename.chop(4); //remove .iso
    filename = work_dir + "/iso-template/" + filename + "/package_list";
    QString cmd = "dpkg -l | grep \"ii\" | awk '{ print $2 }' >" + filename;
    system(cmd.toUtf8());
}

// setup the environment before taking the snapshot
void mxsnapshot::setupEnv()
{
    // checks if work_dir looks OK
    if (!work_dir.contains("/mx-snapshot")) {
        return;
    }
    QDir dir;
    // create an empty fstab file
    system("touch " + work_dir.toUtf8() + "/fstabdummy");
    // mount empty fstab file
    system("mount --bind " + work_dir.toUtf8() + "/fstabdummy /etc/fstab");

    // copy minstall.desktop to Desktop on all accounts
    system("echo /home/*/Desktop | xargs -n1 cp /usr/share/applications/antix/minstall.desktop 2>/dev/null");
    system("echo /home/*/Desktop | xargs -n1 cp /usr/share/applications/mx/minstall.desktop 2>/dev/null");
    system("chmod +x /home/*/Desktop/minstall.desktop");

    // install mx-installer if absent
    if (!checkInstalled("mx-installer")) {
        runCmd("apt-get update");
        if (!checkInstalled("mx-installer")) {
            runCmd("apt-get install mx-installer");
        }
    }

    // setup environment if creating a respin (reset root/demo, remove personal accounts)
    if (reset_accounts) {
        // fix antiX-init start-up
        system("update-rc.d antiX-init defaults >/dev/null 2>&1");

        // copy files that need to be edited to work_dir
        system("cp /etc/passwd " + work_dir.toUtf8());
        system("cp /etc/shadow " + work_dir.toUtf8());
        system("cp /etc/gshadow " + work_dir.toUtf8());
        system("cp /etc/group " + work_dir.toUtf8());
        system("cp /etc/lightdm/lightdm.conf " + work_dir.toUtf8());

        // mount root partition to work directory
        dir.mkpath(work_dir + "/ro_root");
        system(("mount --bind / " + work_dir + "/ro_root").toUtf8());
        // make it read-only
        system(("mount -o remount,ro,bind " + work_dir + "/ro_root").toUtf8());

        // mount empty fstab file
        system("mount --bind " + work_dir.toUtf8() + "/fstabdummy " + work_dir.toUtf8() + "/ro_root/etc/fstab");

        // create work_dir/skel/Desktop
        system("mkdir -p " + work_dir.toUtf8() + "/skel/Desktop");
        // copy /etc/skel on work_dir/skel
        system("rsync -a /etc/skel/ " + work_dir.toUtf8() + "/skel/");
        // copy minstall.desktop to work_dir/skel/Desktop/
        system("cp /usr/share/applications/antix/minstall.desktop " + work_dir.toUtf8() + "/skel/Desktop/Installer.desktop 2>/dev/null");
        system("cp /usr/share/applications/mx/minstall.desktop " + work_dir.toUtf8() + "/skel/Desktop/Installer.desktop 2>/dev/null");
        system("chmod +x " + work_dir.toUtf8() + "/skel/Desktop/Installer.desktop");
        // mount ro_root/etc/skel
        system("mount --bind " + work_dir.toUtf8() + "/skel " + work_dir.toUtf8() + "/ro_root/etc/skel");

        // create dummyhome
        dir.mkpath(work_dir + "/dummyhome");
        // mount dummyhome on ro_root/home
        system(("mount --bind " + work_dir + "/dummyhome " + work_dir + "/ro_root/home").toUtf8());

        // detect additional users
        QStringList users = listUsers();

        // reset user accounts
        createUser1000(); // checks if user with UID=1000 exists if not creates it to be used as "demo"
        resetAccount("demo");
        resetAccount("root");
        resetOtherAccounts(users);
        fixPermissions();

        // mount files over
        system(("mount --bind " + work_dir + "/passwd " + work_dir + "/ro_root/etc/passwd").toUtf8());
        system(("mount --bind " + work_dir + "/shadow " + work_dir + "/ro_root/etc/shadow").toUtf8());
        system(("mount --bind " + work_dir + "/gshadow " + work_dir + "/ro_root/etc/gshadow").toUtf8());
        system(("mount --bind " + work_dir + "/group " + work_dir + "/ro_root/etc/group").toUtf8());
        system(("mount --bind " + work_dir + "/lightdm.conf " + work_dir + "/ro_root/etc/lightdm/lightdm.conf").toUtf8());
    }
}

// fix some permissions
void mxsnapshot::fixPermissions()
{
    system("chgrp shadow " + work_dir.toUtf8() + "/shadow");
    system("chgrp shadow " + work_dir.toUtf8() + "/gshadow");
}

// generates pair root/root and demo/demo passwords and replaces them in ../etc/shadow
void mxsnapshot::resetAccount(QString user)
{
    QString cmd;
    QString sfile = work_dir + "/shadow";
    QString pfile = work_dir + "/passwd";
    QString gfile = work_dir + "/gshadow";
    QString grfile = work_dir + "/group";
    QString lfile = work_dir + "/lightdm.conf";

    // replaces user with UID=1000 with "demo"
    if (user == "demo") {
        QString user1000 = getCmdOut("grep 1000 " + pfile + "| cut -f1 -d':'");
        if (user1000 != "") {
            replaceStringInFile(user1000, "demo", sfile);
            replaceStringInFile(user1000, "demo", pfile);
            replaceStringInFile(user1000, "demo", gfile);
            replaceStringInFile(user1000, "demo", grfile);
            replaceStringInFile(user1000, "demo", lfile);
            system("sed -i -r '/autologin-user=demo/ s/^#+//' " + lfile.toUtf8());
        }
    }
    // replace password in shadow file
    QString pass = getCmdOut("mkpasswd -m sha-512 " + user);
    cmd = QString("awk -F\":\" 'BEGIN{OFS=\":\"}{if ($1 == \"%1\") $2=\"%2\"; print}' " + sfile + ">" + sfile + ".tmp").arg(user).arg(pass);
    system(cmd.toUtf8());
    system("mv " + sfile.toUtf8() + ".tmp " + sfile.toUtf8());
}

// list users that are available in /home and have a login shell
QStringList mxsnapshot::listUsers()
{
    QStringList userList;
    QStringList result;
    QString pfile = work_dir + "/passwd";
    // list all folders in /home with the exception of user uid 1000, demo, and snapshot
    QString user1000 = getCmdOut("grep 1000 " + pfile + "| cut -f1 -d':'");
    QString users = getCmdOut("ls /home | grep -v lost+found | grep -v " + user1000 + " | grep -v demo | grep -v snapshot | grep [a-zA-Z0-9]");
    userList = users.split("\n");
    for (int i = 0; i < userList.size(); ++i) {
        if (userList.at(i) != "") {
            // check if the found user has a regular log in shell
            QString out = getCmdOut("grep " + userList.at(i) + "  " + pfile + "| cut -f7 -d:");
            if (out.endsWith("sh")) {
                result << userList.at(i);
            }
        }
    }
    return result;
}

// resets accounts
void mxsnapshot::resetOtherAccounts(QStringList users)
{
    QString cmd;
    QString sfile = work_dir + "/shadow";
    QString pfile = work_dir + "/passwd";
    QString gfile = work_dir + "/gshadow";
    QString grfile = work_dir + "/group";

    // remove users from the files
    for (int i = 0; i < users.size(); ++i) {
        cmd = QString("sed -i '/^%1:/d' " + sfile).arg(users.at(i));
        system(cmd.toUtf8());
        cmd = QString("sed -i '/^%1:/d' " + pfile).arg(users.at(i));
        system(cmd.toUtf8());
        cmd = QString("sed -i '/^%1:/d' " + gfile).arg(users.at(i));
        system(cmd.toUtf8());
        cmd = QString("sed -i '/^%1:/d' " + grfile).arg(users.at(i));
        system(cmd.toUtf8());
        cmd = QString("sed -i -r -e \"s/:%1(,|$)/:/\" -e \"s/,%1(,|$)/\\1/\" " + grfile).arg(users.at(i));
        system(cmd.toUtf8());
    }
}

// create a demo user with UID=1000 and copies the passwd/shadow files
void mxsnapshot::createUser1000()
{
    QString pfile = work_dir + "/passwd";
    QString user1000 = getCmdOut("grep 1000 " + pfile + "| cut -f1 -d':'");
    if (user1000 == "") {
        // create demo user with UID=1000
        if (system("adduser --disabled-login --uid=1000 --no-create-home --gecos demo demo") == 0) {
            // copy needed files to work directory
            system("cp /etc/passwd " + work_dir.toUtf8());
            system("cp /etc/shadow " + work_dir.toUtf8());
            system("cp /etc/gshadow " + work_dir.toUtf8());
            system("cp /etc/group " + work_dir.toUtf8());
            system("deluser demo");
        }
    }
}

// create squashfs and then the iso
bool mxsnapshot::createIso(QString filename)
{
    QString cmd;
    // add exclusions snapshot dir
    addRemoveExclusion(true, snapshot_dir.absolutePath());

    // squash the filesystem copy
    QDir::setCurrent(work_dir);
    QString source_path = "/";
    if (reset_accounts) {
        source_path = work_dir + "/ro_root";
    }
    cmd = "mksquashfs " + source_path + " iso-template/antiX/linuxfs " + mksq_opt + " -wildcards -ef " + snapshot_excludes.fileName() + " " + session_excludes;
    ui->outputLabel->setText(tr("Squashing filesystem..."));
    if (runCmd(cmd) != 0) {
        QMessageBox::critical(0, tr("Error"), tr("Could not create linuxfs file, please check whether you have enough space on the destination partition."));
        return false;
    }
    makeMd5sum(work_dir + "/iso-template/antiX", "linuxfs");

    // create the iso file
    QDir::setCurrent(work_dir + "/iso-template");
    cmd = "genisoimage -allow-limited-size -l -V MX-Linux-live -R -J -pad -no-emul-boot -boot-load-size 4 -boot-info-table -b boot/isolinux/isolinux.bin -c boot/isolinux/isolinux.cat -o " + snapshot_dir.absolutePath() + "/" + filename + " .";
    ui->outputLabel->setText(tr("Creating CD/DVD image file..."));
    if (runCmd(cmd) != 0) {
        QMessageBox::critical(0, tr("Error"), tr("Could not create ISO file, please check whether you have enough space on the destination partition."));
        return false;
    }

    // make it isohybrid
    if (make_isohybrid == "yes") {
        ui->outputLabel->setText(tr("Making hybrid iso"));
        cmd = "isohybrid " + snapshot_dir.absolutePath() + "/" + filename;
        runCmd(cmd);
    }

    // make md5sum
    if (make_md5sum == "yes") {
        makeMd5sum(snapshot_dir.absolutePath(), filename);
    }
    return true;
}

// create md5sum for different files
void mxsnapshot::makeMd5sum(QString folder, QString filename)
{
    QDir dir;
    QString current = dir.currentPath();
    dir.setCurrent(folder);
    ui->outputLabel->setText(tr("Making md5sum"));
    QString cmd = "md5sum " + filename + ">" + folder + "/" + filename + ".md5";
    runCmd(cmd);
    dir.setCurrent(current);
}

// clean up changes before exit
void mxsnapshot::cleanUp()
{
    QString cmd;
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    ui->outputLabel->setText(tr("Cleaning..."));
    QDir::setCurrent("/");

    // umount empty fstab file
    system("umount /etc/fstab");

    // checks if work_dir looks OK
    if (work_dir.contains("/mx-snapshot")) {
        // clean mount points if resetting accounts
        if (reset_accounts) {
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/passwd >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/shadow >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/gshadow >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/lightdm/lightdm.conf >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/home/demo >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/home >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/fstab >/dev/null 2>&1");
            system("umount " + work_dir.toUtf8() + "/ro_root/etc/skel >/dev/null 2>&1");
            if (system("umount -l " + work_dir.toUtf8() + "/ro_root") != 0) {
                return; // exit if it cannot unmount /ro_root
            }
        }
        system("rm " + work_dir.toUtf8() + "/fstabdummy");
        system("rm -r " + work_dir.toUtf8());
    }
    if (!live) {
        // remove installer icon
        system("rm /home/*/Desktop/minstall.desktop");
        system("rm /etc/skel/Desktop/Installer.desktop");
    }
    ui->outputLabel->clear();
}

// adds or removes exclusion to the exclusion string
void mxsnapshot::addRemoveExclusion(bool add, QString exclusion)
{
    exclusion.remove(0, 1); // remove training slash
    if (add) {
        if ( session_excludes == "" ) {
            session_excludes.append("-e '" + exclusion + "'");
        } else {
            session_excludes.append(" '" + exclusion + "'");
        }
    } else {
        session_excludes.remove(" '" + exclusion + "'");
        if ( session_excludes == "-e" ) {
            session_excludes = "";
        }
    }
}

//// sync process events ////
void mxsnapshot::procStart()
{
    timer->start(100);
    setCursor(QCursor(Qt::BusyCursor));
}

void mxsnapshot::procTime()
{
    int i = ui->progressBar->value() + 1;
    if (i > 100) {
        i = 0;
    }
    ui->progressBar->setValue(i);
    // in live environment and first page, blink text while calculating used disk space
    if (live && (ui->stackedWidget->currentIndex() == 0)) {
        if (ui->progressBar->value()%4 == 0 ) {
            ui->labelUsedSpace->setText("\n " + tr("Please wait."));
        } else {
            ui->labelUsedSpace->setText("\n " + tr("Please wait. Calculating used disk space..."));
        }
    }
}

void mxsnapshot::procDone(int)
{
    timer->stop();
    ui->progressBar->setValue(100);
    setCursor(QCursor(Qt::ArrowCursor));
}

// set proc and timer connections
void mxsnapshot::setConnections()
{
    connect(timer, SIGNAL(timeout()), SLOT(procTime()));
    connect(proc, SIGNAL(started()), SLOT(procStart()));
    connect(proc, SIGNAL(readyReadStandardOutput()), SLOT(onStdoutAvailable()));
    connect(proc, SIGNAL(finished(int)), SLOT(procDone(int)));
}

// disconnect all connections
void mxsnapshot::disconnectAll()
{
    disconnect(timer, SIGNAL(timeout()), 0, 0);
    disconnect(proc, SIGNAL(started()), 0, 0);
    disconnect(proc, SIGNAL(readyReadStandardOutput()), 0, 0);
    disconnect(proc, SIGNAL(finished(int)), 0, 0);
}

//// slots ////

// update output box on Stdout
void mxsnapshot::onStdoutAvailable()
{
    QByteArray output = proc->readAllStandardOutput();
    ui->outputBox->insertPlainText(output);
    QScrollBar *sb = ui->outputBox->verticalScrollBar();
    sb->setValue(sb->maximum());
}


// Next button clicked
void mxsnapshot::on_buttonNext_clicked()
{
    // on first page
    if (ui->stackedWidget->currentIndex() == 0) {
        this->setWindowTitle(tr("Settings"));
        ui->stackedWidget->setCurrentWidget(ui->settingsPage);
        ui->buttonBack->setHidden(false);
        ui->buttonBack->setEnabled(true);
        if (edit_boot_menu == "yes") {
            checkEditor();
        }
        kernel_used = getCmdOut("uname -r");
        ui->stackedWidget->setCurrentWidget(ui->settingsPage);
        ui->label_1->setText(tr("Snapshot will use the following settings:*"));

        ui->label_2->setText(QString("\n" + tr("- Snapshot directory:") + " %1\n" +
                       tr("- Kernel to be used:") + " %2\n").arg(snapshot_dir.absolutePath()).arg(kernel_used));
        ui->label_3->setText(tr("*These settings can be changed by editing: ") + config_file.fileName());

    // on settings page
    } else if (ui->stackedWidget->currentWidget() == ui->settingsPage) {

        int ans = QMessageBox::question(this, tr("Final chance"),
                              tr("Snapshot now has all the information it needs to create an ISO from your running system.") + "\n\n" +
                              tr("It will take some time to finish, depending on the size of the installed system and the capacity of your computer.") + "\n\n" +
                              tr("OK to start?"), QMessageBox::Ok | QMessageBox::Cancel);
        if (ans == QMessageBox::Cancel) {
            return;
        }
        checkDirectories();
        ui->buttonNext->setEnabled(false);
        ui->buttonBack->setEnabled(false);
        ui->stackedWidget->setCurrentWidget(ui->outputPage);
        this->setWindowTitle(tr("Output"));
        copyNewIso();
        QString filename = getFilename();
        ui->outputLabel->clear();
        mkDir(filename);
        savePackageList(filename);

        if (edit_boot_menu == "yes") {
            ans = QMessageBox::question(this, tr("Edit Boot Menu"),
                                  tr("The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot."),
                                     QMessageBox::Yes | QMessageBox::No);
            if (ans == QMessageBox::Yes) {
                this->hide();
                QString cmd = gui_editor.fileName() + " " + work_dir + "/iso-template/boot/isolinux/isolinux.cfg";
                system(cmd.toUtf8());
                this->show();
            }
        }
        setupEnv();
        bool success = createIso(filename);
        cleanUp();
        if (success) {
            QMessageBox::information(this, tr("Success"),tr("All finished!"), QMessageBox::Ok);
            ui->buttonCancel->setText(tr("Close"));
        }
    } else {
        return qApp->quit();
    }
}

void mxsnapshot::on_buttonBack_clicked()
{
    this->setWindowTitle(tr("MX Snapshot"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonNext->setEnabled(true);
    ui->buttonBack->setHidden(true);
    ui->outputBox->clear();
}

void mxsnapshot::on_buttonEditConfig_clicked()
{
    this->hide();
    checkEditor();
    system((gui_editor.fileName() + " " + config_file.fileName()).toUtf8());
    setup();
}

void mxsnapshot::on_buttonEditExclude_clicked()
{
    this->hide();
    checkEditor();
    system((gui_editor.fileName() + " " + snapshot_excludes.fileName()).toUtf8());
    this->show();
}

void mxsnapshot::on_excludeDocuments_toggled(bool checked)
{
    QString exclusion = "/home/*/Documents/*";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_excludeDownloads_toggled(bool checked)
{
    QString exclusion = "/home/*/Downloads/*";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_excludePictures_toggled(bool checked)
{
    QString exclusion = "/home/*/Pictures/*";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_excludeMusic_toggled(bool checked)
{
    QString exclusion = "/home/*/Music/*";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_excludeVideos_toggled(bool checked)
{
    QString exclusion = "/home/*/Videos/*";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_excludeDesktop_toggled(bool checked)
{
    QString exclusion = "/home/*/Desktop/!(minstall.desktop)";
    addRemoveExclusion(checked, exclusion);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void mxsnapshot::on_radioRespin_clicked(bool checked)
{
    if (checked) {
        reset_accounts = true;
        if (!ui->excludeAll->isChecked()) {
            ui->excludeAll->click();
        }
    }
}

void mxsnapshot::on_radioPersonal_clicked(bool checked)
{
    if (checked) {
        if (ui->excludeAll->isChecked()) {
            ui->excludeAll->click();
        }
    }
}


// About button clicked
void mxsnapshot::on_buttonAbout_clicked()
{
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Snapshot"), "<p align=\"center\"><b><h2>" +
                       tr("MX Snapshot") + "</h2></b></p><p align=\"center\">" + tr("Version: ") +
                       version + "</p><p align=\"center\"><h3>" +
                       tr("Program for creating a live-CD from the running system for MX Linux") + "</h3></p><p align=\"center\"><a href=\"http://www.mepiscommunity.org/mx\">http://www.mepiscommunity.org/mx</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        system("mx-viewer file:///usr/share/doc/mx-snapshot/license.html '" + tr("MX Snapshot License").toUtf8() + "'");
}

// Help button clicked
void mxsnapshot::on_buttonHelp_clicked()
{
    system("mx-viewer http://mepiscommunity.org/wiki/help-files/help-mx-save-system-iso-snapshot '" + tr("MX Snapshot Help").toUtf8() + "'");
}

// Select snapshot directory
void mxsnapshot::on_buttonSelectSnapshot_clicked()
{
    QFileDialog dialog;
    this->hide();
    QDir selected = dialog.getExistingDirectory(this, tr("Select Snapshot Directory"), QString(), QFileDialog::ShowDirsOnly);
    if (selected.exists()) {
        snapshot_dir.setPath(selected.absolutePath() + "/snapshot");
        ui->labelSnapshot->setText(tr("The snapshot will be placed in ") + snapshot_dir.absolutePath());
        listFreeSpace();
    }
    this->show();
}

