/**********************************************************************
 *  mainwindow.h
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
#pragma once

#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include "settings.h"
#include "work.h"

class QCommandLineParser;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QDialog, public Settings
{
    Q_OBJECT

public:
    explicit MainWindow(const QCommandLineParser &arg_parser = QCommandLineParser(), QWidget *parent = nullptr);
    ~MainWindow() override;

    [[noreturn]] void cleanUp();
    bool installPackage(const QString &package);
    void checkSaveWork();
    void closeApp();
    void listFreeSpace();
    void listUsedSpace();
    void loadSettings();
    void setConnections();
    void setExclusions();
    void setOtherOptions();
    void setup();

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void disableOutput();
    void displayOutput();
    void outputAvailable(const QString &out);
    void procDone();
    void procStart();
    void processMsg(const QString &msg);
    void processMsgBox(BoxType box_type, const QString &title, const QString &msg);
    void progress();

private slots:
    void btnAbout_clicked();
    void btnBack_clicked();
    void btnCancel_clicked();
    void btnEditExclude_clicked();
    void btnHelp_clicked();
    void btnNext_clicked();
    void btnSelectKernel_clicked();
    void btnSelectSnapshot_clicked();
    void cbCompression_currentIndexChanged();
    void checkMd5_toggled(bool checked);
    void checkSha512_toggled(bool checked);
    void excludeDesktop_toggled(bool checked);
    void excludeDocuments_toggled(bool checked);
    void excludeDownloads_toggled(bool checked);
    void excludeMusic_toggled(bool checked);
    void excludeNetworks_toggled(bool checked);
    void excludePictures_toggled(bool checked);
    void excludeSteam_toggled(bool checked);
    void excludeVideos_toggled(bool checked);
    void excludeVirtualBox_toggled(bool checked);
    void radioPersonal_clicked(bool checked);
    void radioRespin_toggled(bool checked);
    void spinCPU_valueChanged(int arg1);
    void spinThrottle_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool monthly;
    Work work;
    QSettings settings;
};
