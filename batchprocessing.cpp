/**********************************************************************
 *  batchprocessing.cpp
 **********************************************************************
 * Copyright (C) 2020-2024 MX Authors
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

#include "batchprocessing.h"

#include <QDebug>
#include <QRegularExpression>
#include <chrono>

#include "work.h"

using namespace std::chrono_literals;

Batchprocessing::Batchprocessing(const QCommandLineParser &arg_parser, QObject *parent)
    : QObject(parent),
      Settings(arg_parser),
      work(this)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { work.cleanUp(); });
    setConnections();

    if (!checkCompression()) {
        qCritical().noquote() << tr("Error")
                              << tr("Current kernel doesn't support selected compression algorithm, "
                                    "please edit the configuration file and select a different algorithm.");
        return;
    }

    QString path = snapshot_dir;
    qDebug() << "Free space:" << getFreeSpaceStrings(path.remove(QRegularExpression("/snapshot$")));
    if (!arg_parser.isSet("month") && !arg_parser.isSet("override-size")) {
        qDebug() << "Unused space:" << getUsedSpace();
    }

    work.started = true;
    work.e_timer.start();
    if (!checkSnapshotDir() || !checkTempDir()) {
        work.cleanUp();
        return;
    }
    otherExclusions();
    work.setupEnv();
    if (!arg_parser.isSet("month") && !arg_parser.isSet("override-size")) {
        work.checkEnoughSpace();
    }
    work.copyNewIso();
    work.savePackageList(snapshot_name);

    if (edit_boot_menu) {
        qDebug() << tr("The program will pause the build and open the boot menu in your text editor.");
        QString cmd = getEditor() + " \"" + work_dir + "/iso-template/boot/isolinux/isolinux.cfg\"";
        Cmd().run(cmd);
    }
    disconnect(&timer, &QTimer::timeout, nullptr, nullptr);
    work.createIso(snapshot_name);
}

void Batchprocessing::setConnections()
{
    connect(&timer, &QTimer::timeout, this, &Batchprocessing::progress);
    connect(&work.shell, &Cmd::started, this, [this] { timer.start(500ms); });
    connect(&work.shell, &Cmd::done, this, [this] { timer.stop(); });
    connect(&work.shell, &Cmd::outputAvailable, this, [](const QString &out) { qDebug().noquote() << out; });
    connect(&work.shell, &Cmd::errorAvailable, this, [](const QString &out) { qWarning().noquote() << out; });
    connect(&work, &Work::message, [](const QString &out) { qDebug().noquote() << out; });
    connect(&work, &Work::messageBox,
            [](BoxType /*unused*/, const QString &title, const QString &msg) { qDebug().noquote() << title << msg; });
}

void Batchprocessing::progress()
{
    static bool toggle = false;
    qDebug() << "\033[2KProcessing command" << (toggle ? "...\r" : "\r");
    toggle = !toggle;
}

void Batchprocessing::checkNvidiaGraphicsCard()
{
    if (work.shell.run("glxinfo | grep -q NVIDIA")) {
        qDebug() << tr("This computer uses an NVIDIA graphics card. Are you planning to use the "
                       "resulting ISO on the same computer or another computer with an NVIDIA card?")
                        + " yes/no";
        QString response;
        QTextStream stdinStream(stdin);
        stdinStream >> response;

        response = response.toLower();
        if (response == "yes" || response == "y") {
            boot_options += " xorg=nvidia";
            qDebug() << tr("Note: If you use the resulting ISO on a computer without an NVIDIA card, "
                           "you will likely need to remove 'xorg=nvidia' from the boot options.");
        } else {
            qDebug() << tr("Note: If you use the resulting ISO on a computer with an NVIDIA card, "
                           "you may need to add 'xorg=nvidia' to the boot options.");
        }
    }
}
