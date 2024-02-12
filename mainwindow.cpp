/**********************************************************************
 *  mainwindow.cpp
 **********************************************************************
 * Copyright (C) 2015-2024 MX Authors
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTextStream>
#include <QTime>

#include "about.h"
#include "settings.h"
#include "work.h"
#include <chrono>

using namespace std::chrono_literals;

MainWindow::MainWindow(const QCommandLineParser &arg_parser, QWidget *parent)
    : QDialog(parent),
      Settings(arg_parser),
      ui(new Ui::MainWindow),
      work(this)
{
    ui->setupUi(this);
    monthly = arg_parser.isSet("month");
    setConnections();
    setup();
    loadSettings();
    listFreeSpace();
    setExclusions();
    setOtherOptions();
    if (monthly) {
        ui->btnNext->click();
        ui->btnNext->click();
    } else {
        listUsedSpace();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    ui->labelTitleSummary->clear();
    ui->labelSummary->clear();
    ui->labelSnapshotDir->setText(snapshot_dir);
    if (snapshot_name.isEmpty()) {
        ui->lineEditName->setText(getFilename());
    } else {
        ui->lineEditName->setText(snapshot_name);
    }
    ui->textCodename->setText(codename);
    ui->textDistroVersion->setText(distro_version);
    ui->textProjectName->setText(project_name);
    ui->textOptions->setText(boot_options);
    ui->textReleaseDate->setText(release_date);
    ui->textKernel->setText(kernel);
    if (work.shell.getOut("ls -1 /boot/vmlinuz-* | wc -l").toUInt() < 2) {
        ui->btnKernel->setHidden(true);
    }
}

void MainWindow::setOtherOptions()
{
    ui->cbCompression->setCurrentIndex(ui->cbCompression->findText(compression, Qt::MatchStartsWith));
    ui->checkMd5->setChecked(make_md5sum);
    ui->checkSha512->setChecked(make_sha512sum);
    ui->radioRespin->setChecked(reset_accounts);
    ui->spinCPU->setMaximum(static_cast<int>(max_cores));
    ui->spinCPU->setValue(static_cast<int>(cores));
    ui->spinThrottle->setValue(static_cast<int>(throttle));
}

void MainWindow::setConnections()
{
    connect(&timer, &QTimer::timeout, this, &MainWindow::progress);
    connect(&work, &Work::message, this, &MainWindow::processMsg);
    connect(&work, &Work::messageBox, this, &MainWindow::processMsgBox);
    connect(&work.shell, &Cmd::done, this, &MainWindow::procDone);
    connect(&work.shell, &Cmd::errorAvailable, this, [](const QString &out) { qWarning().noquote() << out; });
    connect(&work.shell, &Cmd::outputAvailable, this, [](const QString &out) { qDebug().noquote() << out; });
    connect(&work.shell, &Cmd::started, this, &MainWindow::procStart);
    connect(QApplication::instance(), &QApplication::aboutToQuit, this, [this] { cleanUp(); });
    connect(ui->btnAbout, &QPushButton::clicked, this, &MainWindow::btnAbout_clicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::btnBack_clicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &MainWindow::btnCancel_clicked);
    connect(ui->btnEditExclude, &QPushButton::clicked, this, &MainWindow::btnEditExclude_clicked);
    connect(ui->btnHelp, &QPushButton::clicked, this, &MainWindow::btnHelp_clicked);
    connect(ui->btnKernel, &QPushButton::clicked, this, &MainWindow::btnSelectKernel_clicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &MainWindow::btnNext_clicked);
    connect(ui->btnSelectSnapshot, &QPushButton::clicked, this, &MainWindow::btnSelectSnapshot_clicked);
    connect(ui->cbCompression, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MainWindow::cbCompression_currentIndexChanged);
    connect(ui->checkMd5, &QCheckBox::toggled, this, &MainWindow::checkMd5_toggled);
    connect(ui->checkSha512, &QCheckBox::toggled, this, &MainWindow::checkSha512_toggled);
    connect(ui->excludeAll, &QCheckBox::clicked, this, &MainWindow::excludeAll_clicked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDesktop, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDocuments, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDownloads, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeMusic, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeNetworks, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludePictures, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeSteam, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeVideos, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeVirtualBox, &QCheckBox::setChecked);
    connect(ui->excludeDesktop, &QCheckBox::toggled, this, &MainWindow::excludeDesktop_toggled);
    connect(ui->excludeDocuments, &QCheckBox::toggled, this, &MainWindow::excludeDocuments_toggled);
    connect(ui->excludeDownloads, &QCheckBox::toggled, this, &MainWindow::excludeDownloads_toggled);
    connect(ui->excludeMusic, &QCheckBox::toggled, this, &MainWindow::excludeMusic_toggled);
    connect(ui->excludeNetworks, &QCheckBox::toggled, this, &MainWindow::excludeNetworks_toggled);
    connect(ui->excludePictures, &QCheckBox::toggled, this, &MainWindow::excludePictures_toggled);
    connect(ui->excludeSteam, &QCheckBox::toggled, this, &MainWindow::excludeSteam_toggled);
    connect(ui->excludeVideos, &QCheckBox::toggled, this, &MainWindow::excludeVideos_toggled);
    connect(ui->excludeVirtualBox, &QCheckBox::toggled, this, &MainWindow::excludeVirtualBox_toggled);
    connect(ui->radioPersonal, &QRadioButton::clicked, this, &MainWindow::radioPersonal_clicked);
    connect(ui->radioRespin, &QRadioButton::toggled, this, &MainWindow::radioRespin_toggled);
    connect(ui->spinCPU, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::spinCPU_valueChanged);
    connect(ui->spinThrottle, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::spinThrottle_valueChanged);
}

void MainWindow::setExclusions()
{
    ui->excludeDesktop->setChecked(exclusions.testFlag(Exclude::Desktop));
    ui->excludeDocuments->setChecked(exclusions.testFlag(Exclude::Documents));
    ui->excludeDownloads->setChecked(exclusions.testFlag(Exclude::Downloads));
    ui->excludeMusic->setChecked(exclusions.testFlag(Exclude::Music));
    ui->excludeNetworks->setChecked(exclusions.testFlag(Exclude::Networks));
    ui->excludePictures->setChecked(exclusions.testFlag(Exclude::Pictures));
    ui->excludeSteam->setChecked(exclusions.testFlag(Exclude::Steam));
    ui->excludeVideos->setChecked(exclusions.testFlag(Exclude::Videos));
    ui->excludeVirtualBox->setChecked(exclusions.testFlag(Exclude::VirtualBox));
}

void MainWindow::setup()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    setWindowFlags(Qt::Window); // for the close, min and max buttons
    QFont font("monospace");
    font.setStyleHint(QFont::Monospace);
    ui->outputBox->setFont(font);
    ui->outputBox->setReadOnly(true);

    setWindowTitle(tr("MX Snapshot"));
    ui->btnBack->setHidden(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnCancel->setEnabled(true);
    ui->btnNext->setEnabled(true);
    ui->cbCompression->blockSignals(true);
    ui->cbCompression->addItems({"lz4 - " + tr("fastest, worst compression"), "lzo - " + tr("fast, worse compression"),
                                 "gzip - " + tr("slow, better compression"), "zstd - " + tr("best compromise"),
                                 "xz - " + tr("slowest, best compression")});
    ui->cbCompression->blockSignals(false);
    if (Settings::getDebianVerNum() < Release::Bookworm) {
        ui->labelThrottle->hide();
        ui->spinThrottle->hide();
    }

    show();
}

void MainWindow::listUsedSpace()
{
    show();
    ui->btnNext->setDisabled(true);
    ui->btnCancel->setDisabled(true);
    ui->btnSelectSnapshot->setDisabled(true);
    ui->btnNext->setEnabled(true);
    ui->btnCancel->setEnabled(true);
    ui->btnSelectSnapshot->setEnabled(true);
    ui->labelUsedSpace->setText(getUsedSpace());
}

void MainWindow::listFreeSpace()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QString path = snapshot_dir;
    path.remove(QRegularExpression("/snapshot$"));
    QString free_space = getFreeSpaceStrings(path);
    ui->labelFreeSpace->clear();
    ui->labelFreeSpace->setText("- " + tr("Free space on %1, where snapshot folder is placed: ").arg(path) + free_space
                                + "\n");
    ui->labelDiskSpaceHelp->setText(
        tr("The free space should be sufficient to hold the compressed data from / and /home\n\n"
           "      If necessary, you can create more available space by removing previous snapshots and saved copies: "
           "%1 snapshots are taking up %2 of disk space.")
            //        tr("The free space should be sufficient to hold the compressed data from / and /home\n\n"
            //           "      If necessary, you can create more available space\n"
            //           "      by removing previous snapshots and saved copies:\n"
            //           "      %1 snapshots are taking up %2 of disk space.\n")
            .arg(QString::number(getSnapshotCount()), getSnapshotSize()));
}

bool MainWindow::installPackage(const QString &package)
{
    setWindowTitle(tr("Installing ") + package);
    ui->outputLabel->setText(tr("Installing ") + package);
    ui->outputBox->clear();
    ui->btnNext->setDisabled(true);
    ui->btnBack->setDisabled(true);
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    displayOutput();
    if (!work.installPackage(package)) {
        disableOutput();
        return false;
    }
    disableOutput();
    return true;
}

void MainWindow::cleanUp()
{
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    work.cleanUp();
}

void MainWindow::procStart()
{
    timer.start(500ms);
    setCursor(QCursor(Qt::BusyCursor));
}

void MainWindow::processMsgBox(BoxType box_type, const QString &title, const QString &msg)
{
    qDebug().noquote() << title << msg;
    switch (box_type) {
    case BoxType::warning:
        QMessageBox::warning(this, title, msg);
        break;
    case BoxType::critical:
        QMessageBox::critical(this, title, msg);
        break;
    case BoxType::question:
        QMessageBox::question(this, title, msg);
        break;
    case BoxType::information:
        QMessageBox::information(this, title, msg);
        break;
    }
}

void MainWindow::processMsg(const QString &msg)
{
    qDebug().noquote() << msg;
    ui->outputLabel->setText(msg);
}

void MainWindow::procDone()
{
    timer.stop();
    ui->progressBar->setValue(ui->progressBar->maximum());
    setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::displayOutput()
{
    connect(&work.shell, &Cmd::outputAvailable, this, &MainWindow::outputAvailable);
    connect(&work.shell, &Cmd::errorAvailable, this, &MainWindow::outputAvailable);
}

void MainWindow::disableOutput()
{
    disconnect(&work.shell, &Cmd::outputAvailable, this, nullptr);
    disconnect(&work.shell, &Cmd::errorAvailable, this, nullptr);
}

void MainWindow::outputAvailable(const QString &out)
{
    ui->outputBox->moveCursor(QTextCursor::End);
    if (out.startsWith(QLatin1String("\r"))) {
        ui->outputBox->moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);
        ui->outputBox->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }
    ui->outputBox->insertPlainText(out);
    ui->outputBox->verticalScrollBar()->setValue(ui->outputBox->verticalScrollBar()->maximum());
}

void MainWindow::progress()
{
    ui->progressBar->setValue((ui->progressBar->value() + 1) % ui->progressBar->maximum() + 1);

    // In live environment and first page, blink text while calculating used disk space
    if (live && (ui->stackedWidget->currentIndex() == 0)) {
        if (ui->progressBar->value() % 4 == 0) {
            ui->labelUsedSpace->setText("\n " + tr("Please wait."));
        } else {
            ui->labelUsedSpace->setText("\n " + tr("Please wait. Calculating used disk space..."));
        }
    }
}

void MainWindow::btnNext_clicked()
{
    QString file_name = ui->lineEditName->text();
    if (!file_name.endsWith(".iso")) {
        file_name += ".iso";
    }

    if (QFile::exists(snapshot_dir + "/" + file_name)) {
        QMessageBox::critical(
            this, tr("Error"),
            tr("Output file %1 already exists. Please use another file name, or delete the existent file.")
                .arg(snapshot_dir + "/" + file_name));
        return;
    }

    // On first page
    if (ui->stackedWidget->currentWidget() == ui->selectionPage) {
        setWindowTitle(tr("Settings"));
        ui->stackedWidget->setCurrentWidget(ui->settingsPage);
        ui->btnBack->setHidden(false);
        ui->btnBack->setEnabled(true);
        selectKernel();
        ui->labelTitleSummary->setText(tr("Snapshot will use the following settings:"));

        ui->labelSummary->setText("\n" + tr("- Snapshot directory:") + " " + snapshot_dir + "\n" + "- "
                                  + tr("Snapshot name:") + " " + file_name + "\n" + tr("- Kernel to be used:") + " "
                                  + kernel + "\n");
        codename = ui->textCodename->text();
        distro_version = ui->textDistroVersion->text();
        project_name = ui->textProjectName->text();
        full_distro_name = project_name + "-" + distro_version + "_" + QString(x86 ? "386" : "x64");
        boot_options = ui->textOptions->text();
        release_date = ui->textReleaseDate->text();
        // On settings page
    } else if (ui->stackedWidget->currentWidget() == ui->settingsPage) {
        if (!checkCompression()) {
            processMsgBox(BoxType::critical, tr("Error"),
                          tr("Current kernel doesn't support selected compression algorithm, please edit the "
                             "configuration file and select a different algorithm."));
            return;
        }

        QMessageBox messageBox(this);
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setWindowTitle(tr("Final chance"));
        messageBox.setText(
            tr("Snapshot now has all the information it needs to create an ISO from your running system.") + "\n\n"
            + tr("It will take some time to finish, depending on the size of the installed system and the capacity of "
                 "your computer.")
            + "\n\n" + tr("OK to start?"));
        messageBox.addButton(QMessageBox::Ok);
        auto *pushCancel = messageBox.addButton(QMessageBox::Cancel);
        auto *checkShutdown = new QCheckBox(this);
        checkShutdown->setText(tr("Shutdown computer when done."));
        if (shutdown) {
            checkShutdown->setCheckState(Qt::Checked);
        }
        messageBox.setCheckBox(checkShutdown);
        messageBox.exec();
        if (messageBox.clickedButton() == pushCancel) {
            return;
        }
        shutdown = checkShutdown->isChecked();

        work.started = true;
        work.e_timer.start();
        if (!checkSnapshotDir()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not create working directory. ") + snapshot_dir);
            cleanUp();
        }
        if (!checkTempDir()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not create temporary directory. ") + snapshot_dir);
            cleanUp();
        }

        otherExclusions();
        ui->btnNext->setEnabled(false);
        ui->btnBack->setEnabled(false);
        ui->stackedWidget->setCurrentWidget(ui->outputPage);
        setWindowTitle(tr("Output"));
        ui->outputBox->clear();
        work.setupEnv();
        if (!monthly && !override_size) {
            work.checkEnoughSpace();
        }
        work.copyNewIso();
        ui->outputLabel->setText(QLatin1String(""));
        work.savePackageList(file_name);

        if (edit_boot_menu) {
            if (QMessageBox::Yes
                == QMessageBox::question(
                    this, tr("Edit Boot Menu"),
                    tr("The program will now pause to allow you to edit any files in the work directory. "
                       "Select Yes to edit the boot menu or select No to bypass this step and continue creating the "
                       "snapshot."),
                    QMessageBox::Yes | QMessageBox::No)) {
                hide();
                QString cmd = getEditor() + " \"" + work_dir + "/iso-template/boot/isolinux/isolinux.cfg\"";
                work.shell.run(cmd);
                show();
            }
        }

        displayOutput();
        work.createIso(file_name);
        ui->btnCancel->setText(tr("Close"));
    } else {
        QApplication::quit();
    }
}

void MainWindow::btnSelectKernel_clicked()
{
    QString selected = QFileDialog::getOpenFileName(this, tr("Select kernel"), "/boot", "vmlinuz-*");
    if (QFile::exists(selected)) {
        ui->textKernel->setText(selected.remove(QRegularExpression("^/boot/vmlinuz-")));
        kernel = ui->textKernel->text();
    }
}

void MainWindow::btnBack_clicked()
{
    setWindowTitle(tr("MX Snapshot"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnNext->setEnabled(true);
    ui->btnBack->setHidden(true);
    ui->outputBox->clear();
}

void MainWindow::btnEditExclude_clicked()
{
    hide();
    work.shell.run(getEditor() + " " + snapshot_excludes.fileName());
    show();
}

void MainWindow::excludeDocuments_toggled(bool checked)
{
    excludeDocuments(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeDownloads_toggled(bool checked)
{
    excludeDownloads(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludePictures_toggled(bool checked)
{
    excludePictures(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeMusic_toggled(bool checked)
{
    excludeMusic(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeVideos_toggled(bool checked)
{
    excludeVideos(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeDesktop_toggled(bool checked)
{
    excludeDesktop(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::radioRespin_toggled(bool checked)
{
    reset_accounts = checked;
    if (checked && !ui->excludeAll->isChecked()) {
        ui->excludeAll->click();
    }
}

void MainWindow::radioPersonal_clicked(bool checked)
{
    reset_accounts = !checked;
    if (checked && ui->excludeAll->isChecked()) {
        ui->excludeAll->click();
    }
}

void MainWindow::btnAbout_clicked()
{
    hide();
    displayAboutMsgBox(
        tr("About %1").arg(QApplication::applicationDisplayName()),
        "<p align=\"center\"><b><h2>" + QApplication::applicationDisplayName() + "</h2></b></p><p align=\"center\">"
            + tr("Version: ") + QApplication::applicationVersion() + "</p><p align=\"center\"><h3>"
            + tr("Program for creating a live-CD from the running system for MX Linux")
            + R"(</h3></p><p align="center"><a href="http://mxlinux.org">http://mxlinux.org</a><br /></p><p align="center">)"
            + tr("Copyright (c) MX Linux") + "<br /><br /></p>",
        QStringLiteral("/usr/share/doc/mx-snapshot/license.html"),
        tr("%1 License").arg(QApplication::applicationDisplayName()));
    show();
}

void MainWindow::btnHelp_clicked()
{
    QLocale locale;
    QString lang = locale.bcp47Name();

    QString url {"/usr/share/doc/mx-snapshot/mx-snapshot.html"};

    if (lang.startsWith(QLatin1String("fr"))) {
        url = "https://mxlinux.org/french-wiki/help-files-fr/help-mx-instantane";
    }
    displayDoc(url, tr("%1 Help").arg(windowTitle()));
}

void MainWindow::btnSelectSnapshot_clicked()
{
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select Snapshot Directory"), QString(),
                                                         QFileDialog::ShowDirsOnly);
    if (!selected.isEmpty()) {
        snapshot_dir = selected + "/snapshot";
        ui->labelSnapshotDir->setText(snapshot_dir);
        listFreeSpace();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        closeApp();
    }
}

void MainWindow::closeApp()
{
    // Ask for confirmation when on outputPage and not done
    if (ui->stackedWidget->currentWidget() == ui->outputPage && !work.done) {
        if (QMessageBox::Yes
            != QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to quit the application?"),
                                     QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }
    cleanUp();
}

void MainWindow::btnCancel_clicked()
{
    closeApp();
}

void MainWindow::cbCompression_currentIndexChanged()
{
    QString comp = ui->cbCompression->currentText().section(" ", 0, 0);
    settings.setValue("compression", comp);
    compression = comp;
}

void MainWindow::excludeNetworks_toggled(bool checked)
{
    excludeNetworks(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::checkMd5_toggled(bool checked)
{
    settings.setValue("make_md5sum", checked ? "yes" : "no");
    make_md5sum = checked;
}

void MainWindow::checkSha512_toggled(bool checked)
{
    settings.setValue("make_sha512sum", checked ? "yes" : "no");
    make_sha512sum = checked;
}

void MainWindow::excludeAll_clicked(bool checked)
{
    qDebug() << "EXCLUDE ALL" << checked;
    excludeDesktop_toggled(checked);
    excludeDocuments_toggled(checked);
    excludeDownloads_toggled(checked);
    excludeMusic_toggled(checked);
    excludeNetworks_toggled(checked);
    excludePictures_toggled(checked);
    excludeSteam_toggled(checked);
    excludeVideos_toggled(checked);
    excludeVirtualBox_toggled(checked);
}

void MainWindow::excludeSteam_toggled(bool checked)
{
    excludeSteam(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeVirtualBox_toggled(bool checked)
{
    excludeVirtualBox(checked);
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::spinCPU_valueChanged(int arg1)
{
    settings.setValue("cores", arg1);
    cores = arg1;
}

void MainWindow::spinThrottle_valueChanged(int arg1)
{
    settings.setValue("throttle", arg1);
    throttle = arg1;
}
