/**********************************************************************
 *  mainwindow.cpp
 **********************************************************************
 * Copyright (C) 2015-2025 MX Authors
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

#include <QCalendarWidget>
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

MainWindow::MainWindow(Settings *settings, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::MainWindow),
      settings(settings),
      work(settings, this)
{
    ui->setupUi(this);
    setConnections();
    setup();
    loadSettings();
    listFreeSpace();
    setExclusions();
    setOtherOptions();
    if (settings->monthly) {
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
    ui->labelSnapshotDir->setText(settings->snapshot_dir);
    if (settings->snapshot_name.isEmpty()) {
        ui->lineEditName->setText(settings->getFilename());
    } else {
        ui->lineEditName->setText(settings->snapshot_name);
    }
    ui->textCodename->setText(settings->codename);
    ui->textDistroVersion->setText(settings->distro_version);
    ui->textProjectName->setText(settings->project_name);
    ui->textOptions->setText(settings->boot_options);
    ui->pushReleaseDate->setText(settings->release_date);
    QDir bootDir("/boot");
    QStringList kernelFiles = bootDir.entryList({"vmlinuz-*"}, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    std::transform(kernelFiles.begin(), kernelFiles.end(), kernelFiles.begin(),
                   [](const QString &file) { return file.mid(QStringLiteral("vmlinuz-").length()); });
    ui->comboLiveKernel->addItems(kernelFiles);
    ui->comboLiveKernel->setCurrentText(settings->kernel);
}

void MainWindow::setOtherOptions()
{
    ui->cbCompression->setCurrentIndex(ui->cbCompression->findText(settings->compression, Qt::MatchStartsWith));
    ui->checkMd5->setChecked(settings->make_md5sum);
    ui->checkSha512->setChecked(settings->make_sha512sum);
    ui->radioRespin->setChecked(settings->reset_accounts);
    ui->spinCPU->setMaximum(static_cast<int>(settings->max_cores));
    ui->spinCPU->setValue(static_cast<int>(settings->cores));
    ui->spinThrottle->setValue(static_cast<int>(settings->throttle));
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
    connect(ui->btnNext, &QPushButton::clicked, this, &MainWindow::btnNext_clicked);
    connect(ui->btnSelectSnapshot, &QPushButton::clicked, this, &MainWindow::btnSelectSnapshot_clicked);
    connect(ui->cbCompression, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MainWindow::cbCompression_currentIndexChanged);
    connect(ui->checkMd5, &QCheckBox::toggled, this, &MainWindow::checkMd5_toggled);
    connect(ui->checkSha512, &QCheckBox::toggled, this, &MainWindow::checkSha512_toggled);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDesktop, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDocuments, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeDownloads, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeFlatpaks, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeMusic, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeNetworks, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludePictures, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeSteam, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeVideos, &QCheckBox::setChecked);
    connect(ui->excludeAll, &QCheckBox::clicked, ui->excludeVirtualBox, &QCheckBox::setChecked);
    connect(ui->excludeDesktop, &QCheckBox::toggled, this, &MainWindow::excludeDesktop_toggled);
    connect(ui->excludeDocuments, &QCheckBox::toggled, this, &MainWindow::excludeDocuments_toggled);
    connect(ui->excludeDownloads, &QCheckBox::toggled, this, &MainWindow::excludeDownloads_toggled);
    connect(ui->excludeFlatpaks, &QCheckBox::toggled, this, &MainWindow::excludeFlatpaks_toggled);
    connect(ui->excludeMusic, &QCheckBox::toggled, this, &MainWindow::excludeMusic_toggled);
    connect(ui->excludeNetworks, &QCheckBox::toggled, this, &MainWindow::excludeNetworks_toggled);
    connect(ui->excludePictures, &QCheckBox::toggled, this, &MainWindow::excludePictures_toggled);
    connect(ui->excludeSteam, &QCheckBox::toggled, this, &MainWindow::excludeSteam_toggled);
    connect(ui->excludeVideos, &QCheckBox::toggled, this, &MainWindow::excludeVideos_toggled);
    connect(ui->excludeVirtualBox, &QCheckBox::toggled, this, &MainWindow::excludeVirtualBox_toggled);
    connect(ui->pushReleaseDate, &QPushButton::clicked, this, [this] {
        QCalendarWidget *calendarWidget = new QCalendarWidget(this);
        calendarWidget->setWindowTitle(tr("Select Release Date"));
        calendarWidget->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
        connect(calendarWidget, &QCalendarWidget::activated, this, [this, calendarWidget](const QDate date) {
            ui->pushReleaseDate->setText(date.toString("MMMM dd, yyyy"));
            calendarWidget->deleteLater();
        });
        calendarWidget->show();
    });
    connect(ui->radioPersonal, &QRadioButton::clicked, this, &MainWindow::radioPersonal_clicked);
    connect(ui->radioRespin, &QRadioButton::toggled, this, &MainWindow::radioRespin_toggled);
    connect(ui->spinCPU, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::spinCPU_valueChanged);
    connect(ui->spinThrottle, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::spinThrottle_valueChanged);
}

void MainWindow::setExclusions()
{
    QVector<QPair<QCheckBox *, Settings::Exclude>> exclusionPairs
        = {{ui->excludeDesktop, Settings::Exclude::Desktop},     {ui->excludeDocuments, Settings::Exclude::Documents},
           {ui->excludeDownloads, Settings::Exclude::Downloads}, {ui->excludeFlatpaks, Settings::Exclude::Flatpaks},
           {ui->excludeMusic, Settings::Exclude::Music},         {ui->excludeNetworks, Settings::Exclude::Networks},
           {ui->excludePictures, Settings::Exclude::Pictures},   {ui->excludeSteam, Settings::Exclude::Steam},
           {ui->excludeVideos, Settings::Exclude::Videos},       {ui->excludeVirtualBox, Settings::Exclude::VirtualBox}};

    for (const auto &pair : exclusionPairs) {
        pair.first->setChecked(settings->exclusions.testFlag(pair.second));
    }
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
    ui->labelUsedSpace->setText(settings->getUsedSpace());
}

void MainWindow::listFreeSpace()
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    QString path = settings->snapshot_dir;
    path.remove(QRegularExpression("/snapshot$"));
    QString free_space = settings->getFreeSpaceStrings(path);
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
            .arg(QString::number(settings->getSnapshotCount()), settings->getSnapshotSize()));
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

    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);

    static const QMap<BoxType, QMessageBox::Icon> iconMap = {{BoxType::warning, QMessageBox::Warning},
                                                             {BoxType::critical, QMessageBox::Critical},
                                                             {BoxType::question, QMessageBox::Question},
                                                             {BoxType::information, QMessageBox::Information}};

    msgBox.setIcon(iconMap.value(box_type, QMessageBox::NoIcon));
    msgBox.exec();
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
    if (out.startsWith("\r")) {
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
    if (settings->live && (ui->stackedWidget->currentIndex() == 0)) {
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
    appendIsoExtension(file_name);

    if (QFile::exists(settings->snapshot_dir + "/" + file_name)) {
        showErrorMessageBox(settings->snapshot_dir + "/" + file_name);
        return;
    }

    if (ui->stackedWidget->currentWidget() == ui->selectionPage) {
        handleSelectionPage(file_name);
    } else if (ui->stackedWidget->currentWidget() == ui->settingsPage) {
        handleSettingsPage(file_name);
    } else {
        QApplication::quit();
    }
}

void MainWindow::appendIsoExtension(QString &file_name) const
{
    if (!file_name.endsWith(".iso")) {
        file_name += ".iso";
    }
}

void MainWindow::showErrorMessageBox(const QString &file_path)
{
    QMessageBox::critical(
        this, tr("Error"),
        tr("Output file %1 already exists. Please use another file name, or delete the existent file.").arg(file_path));
}

void MainWindow::handleSelectionPage(const QString &file_name)
{
    setWindowTitle(tr("Settings"));
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
    ui->btnBack->setHidden(false);
    ui->btnBack->setEnabled(true);
    settings->kernel = ui->comboLiveKernel->currentText();
    settings->selectKernel();
    ui->labelTitleSummary->setText(tr("Snapshot will use the following settings:"));

    ui->labelSummary->setText("\n" + tr("- Snapshot directory:") + " " + settings->snapshot_dir + "\n" + "- "
                              + tr("Snapshot name:") + " " + file_name + "\n" + tr("- Kernel to be used:") + " "
                              + settings->kernel + "\n");
    settings->codename = ui->textCodename->text();
    settings->distro_version = ui->textDistroVersion->text();
    settings->project_name = ui->textProjectName->text();
    settings->full_distro_name = settings->project_name + "-" + settings->distro_version + "_" + QString(settings->x86 ? "386" : "x64");
    settings->boot_options = ui->textOptions->text();
    settings->release_date = ui->pushReleaseDate->text();
    checkNvidiaGraphicsCard();
}

void MainWindow::checkNvidiaGraphicsCard()
{
    static bool hasRun = false;
    QString currentOptions = ui->textOptions->text();

    if (hasRun || currentOptions.contains("xorg=nvidia")) {
        return;
    }

    if (work.shell.run("glxinfo | grep -q NVIDIA")) {
        if (QMessageBox::Yes
            == QMessageBox::question(this, tr("NVIDIA Detected"),
                                     tr("This computer uses an NVIDIA graphics card. Are you planning to use the "
                                        "resulting ISO on the same computer or another computer with an NVIDIA card?"),
                                     QMessageBox::Yes | QMessageBox::No)) {
            ui->textOptions->setText(currentOptions.isEmpty() ? "xorg=nvidia" : currentOptions + " xorg=nvidia");
            QMessageBox::information(this, tr("NVIDIA Selected"),
                                     tr("Note: If you use the resulting ISO on a computer without an NVIDIA card, "
                                        "you will likely need to remove 'xorg=nvidia' from the boot options."));
        } else {
            QMessageBox::information(this, tr("NVIDIA Detected"),
                                     tr("Note: If you use the resulting ISO on a computer with an NVIDIA card, "
                                        "you may need to add 'xorg=nvidia' to the boot options."));
        }
    }
    hasRun = true;
}

void MainWindow::handleSettingsPage(const QString &file_name)
{
    if (!settings->checkCompression()) {
        processMsgBox(BoxType::critical, tr("Error"),
                      tr("Current kernel doesn't support selected compression algorithm, please edit the "
                         "configuration file and select a different algorithm."));
        return;
    }

    if (!confirmStart()) {
        return;
    }

    work.startTimer();
    if (!settings->checkSnapshotDir() || !settings->checkTempDir()) {
        cleanUp();
        return;
    }

    applyExclusions();
    prepareForOutput(file_name);
}

bool MainWindow::confirmStart()
{
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
    if (settings->shutdown) {
        checkShutdown->setCheckState(Qt::Checked);
    }
    messageBox.setCheckBox(checkShutdown);
    messageBox.exec();
    if (messageBox.clickedButton() == pushCancel) {
        return false;
    }
    settings->shutdown = checkShutdown->isChecked();
    return true;
}

void MainWindow::applyExclusions()
{
    settings->excludeDesktop(ui->excludeDesktop->isChecked());
    settings->excludeDocuments(ui->excludeDocuments->isChecked());
    settings->excludeDownloads(ui->excludeDownloads->isChecked());
    settings->excludeFlatpaks(ui->excludeFlatpaks->isChecked());
    settings->excludeMusic(ui->excludeMusic->isChecked());
    settings->excludeNetworks(ui->excludeNetworks->isChecked());
    settings->excludePictures(ui->excludePictures->isChecked());
    settings->excludeSteam(ui->excludeSteam->isChecked());
    settings->excludeVideos(ui->excludeVideos->isChecked());
    settings->excludeVirtualBox(ui->excludeVirtualBox->isChecked());
    settings->otherExclusions();
}

void MainWindow::prepareForOutput(const QString &file_name)
{
    ui->btnNext->setEnabled(false);
    ui->btnBack->setEnabled(false);
    ui->stackedWidget->setCurrentWidget(ui->outputPage);
    setWindowTitle(tr("Output"));
    ui->outputBox->clear();
    work.setupEnv();
    if (!settings->monthly && !settings->override_size) {
        work.checkEnoughSpace();
    }
    work.copyNewIso();
    ui->outputLabel->clear();
    work.savePackageList(file_name);

    if (settings->edit_boot_menu) {
        editBootMenu();
    }

    displayOutput();
    work.createIso(file_name);
    ui->btnCancel->setText(tr("Close"));
}

void MainWindow::editBootMenu()
{
    if (QMessageBox::Yes
        == QMessageBox::question(
            this, tr("Edit Boot Menu"),
            tr("The program will now pause to allow you to edit any files in the work directory. "
               "Select Yes to edit the boot menu or select No to bypass this step and continue creating the "
               "snapshot."),
            QMessageBox::Yes | QMessageBox::No)) {
        hide();
        QString cmd = settings->getEditor() + " \"" + settings->work_dir + "/iso-template/boot/isolinux/isolinux.cfg\"";
        work.shell.run(cmd);
        show();
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
    work.shell.run(settings->getEditor() + " " + settings->snapshot_excludes.fileName());
    show();
}

void MainWindow::excludeDocuments_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeDownloads_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeFlatpaks_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludePictures_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeMusic_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeVideos_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeDesktop_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::radioRespin_toggled(bool checked)
{
    settings->reset_accounts = checked;
    if (checked && !ui->excludeAll->isChecked()) {
        ui->excludeAll->click();
    }
}

void MainWindow::radioPersonal_clicked(bool checked)
{
    settings->reset_accounts = !checked;
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

    if (lang.startsWith("fr")) {
        url = "https://mxlinux.org/french-wiki/help-files-fr/help-mx-instantane";
    }
    displayDoc(url, tr("%1 Help").arg(windowTitle()));
}

void MainWindow::btnSelectSnapshot_clicked()
{
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select Snapshot Directory"), QString(),
                                                         QFileDialog::ShowDirsOnly);
    if (!selected.isEmpty()) {
        settings->snapshot_dir = selected + "/snapshot";
        ui->labelSnapshotDir->setText(settings->snapshot_dir);
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
    if (ui->stackedWidget->currentWidget() == ui->outputPage && !work.isDone()) {
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
    qt_settings.setValue("compression", comp);
    settings->compression = comp;
}

void MainWindow::excludeNetworks_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::checkMd5_toggled(bool checked)
{
    qt_settings.setValue("make_md5sum", checked ? "yes" : "no");
    settings->make_md5sum = checked;
}

void MainWindow::checkSha512_toggled(bool checked)
{
    qt_settings.setValue("make_sha512sum", checked ? "yes" : "no");
    settings->make_sha512sum = checked;
}

void MainWindow::excludeSteam_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::excludeVirtualBox_toggled(bool checked)
{
    if (!checked) {
        ui->excludeAll->setChecked(false);
    }
}

void MainWindow::spinCPU_valueChanged(int arg1)
{
    qt_settings.setValue("cores", arg1);
    settings->cores = arg1;
}

void MainWindow::spinThrottle_valueChanged(int arg1)
{
    qt_settings.setValue("throttle", arg1);
    settings->throttle = arg1;
}
