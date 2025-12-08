/**********************************************************************
 *  batchprocessing.cpp
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

#include "batchprocessing.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>
#include <chrono>
#include <utime.h>

#include "work.h"

using namespace std::chrono_literals;

Batchprocessing::Batchprocessing(Settings *settings, QObject *parent)
    : QObject(parent),
      settings(settings),
      work(settings, this)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { work.cleanUp(); });
    setConnections();

    // Check updated excludes before any work
    checkUpdatedDefaultExcludesCli();

    if (!settings->checkCompression()) {
        qCritical().noquote() << tr("Error")
                              << tr("Current kernel doesn't support selected compression algorithm, "
                                    "please edit the configuration file and select a different algorithm.");
        return;
    }

    QString path = settings->snapshot_dir;
    qDebug() << "Free space:" << settings->getFreeSpaceStrings(path.remove(QRegularExpression("/snapshot$")));
    if (!settings->monthly && !settings->override_size) {
        qDebug() << "Unused space:" << settings->getUsedSpace();
    }

    work.startTimer();
    if (!settings->checkSnapshotDir() || !settings->checkTempDir()) {
        work.cleanUp();
        return;
    }
    settings->otherExclusions();
    work.setupEnv();
    if (!settings->monthly && !settings->override_size) {
        work.checkEnoughSpace();
    }
    work.copyNewIso();
    work.savePackageList(settings->snapshot_name);

    if (settings->edit_boot_menu) {
        qDebug() << tr("The program will pause the build and open the boot menu in your text editor.");
        QString cmd = settings->getEditor() + " \"" + settings->work_dir + "/iso-template/boot/grub/grub.cfg\" \""
                      + settings->work_dir + "/iso-template/boot/syslinux/syslinux.cfg\" \"" + settings->work_dir
                      + "/iso-template/boot/isolinux/isolinux.cfg\"";
        Cmd().run(cmd);
    }
    disconnect(&timer, &QTimer::timeout, nullptr, nullptr);
    work.createIso(settings->snapshot_name);
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

bool Batchprocessing::isSourceExcludesNewer(QString &diffOutput) const
{
    const QString configuredPath = settings->snapshot_excludes.fileName();
    const QString sourcePath = settings->getExcludesSourcePath();

    qDebug().noquote() << "CLI excludes check:"
                       << "configured=" << configuredPath
                       << "source=" << sourcePath;

    if (sourcePath.isEmpty() || configuredPath.isEmpty()) {
        qDebug() << "CLI excludes check: empty path(s)";
        return false;
    }

    const QFileInfo configuredInfo(configuredPath);
    const QFileInfo sourceInfo(sourcePath);

    if (!configuredInfo.exists() || !sourceInfo.exists()) {
        qDebug() << "CLI excludes check: missing files"
                 << configuredInfo.exists() << sourceInfo.exists();
        return false;
    }

    qDebug() << "CLI excludes check: mtime configured" << configuredInfo.lastModified()
             << "source" << sourceInfo.lastModified();

    if (sourceInfo.lastModified() <= configuredInfo.lastModified()) {
        qDebug() << "CLI excludes check: source not newer";
        return false;
    }

    QProcess diffProcess;
    diffProcess.start("diff", {"--unified", configuredPath, sourcePath});
    if (!diffProcess.waitForFinished()) {
        qWarning() << "Unable to compare excludes files with diff:" << configuredPath << sourcePath;
        return false;
    }

    const int diffResult = diffProcess.exitCode();
    qDebug() << "CLI excludes check: diff exit code" << diffResult;
    if (diffResult == 0) {
        return false;
    }
    if (diffResult != 1) {
        qWarning() << "Unable to compare excludes files with diff:" << configuredPath << sourcePath;
        return false;
    }

    diffOutput = QString::fromUtf8(diffProcess.readAllStandardOutput());
    if (diffOutput.isEmpty()) {
        diffOutput = QString::fromUtf8(diffProcess.readAllStandardError());
    }
    if (diffOutput.isEmpty()) {
        diffOutput = tr("No diff output available.");
    }
    return true;
}

QString Batchprocessing::colorizeDiffAnsi(const QString &diff) const
{
    static const QString green = "\033[32m";
    static const QString red = "\033[31m";
    static const QString blue = "\033[34m";
    static const QString gray = "\033[90m";
    static const QString reset = "\033[0m";

    QStringList lines = diff.split('\n');
    QString colored;
    for (const QString &line : lines) {
        if (line.startsWith("+++ ") || line.startsWith("--- ")) {
            colored += gray + line + reset + '\n';
        } else if (line.startsWith("@@")) {
            colored += blue + line + reset + '\n';
        } else if (line.startsWith('+')) {
            colored += green + line + reset + '\n';
        } else if (line.startsWith('-')) {
            colored += red + line + reset + '\n';
        } else {
            colored += line + '\n';
        }
    }
    return colored;
}

bool Batchprocessing::resetCustomExcludesCli(const QString &configuredPath, const QString &sourcePath) const
{
    if (sourcePath.isEmpty() || configuredPath.isEmpty()) {
        return false;
    }

    if (!QFileInfo::exists(sourcePath)) {
        qWarning().noquote() << tr("Default exclusion file not found at %1.").arg(sourcePath);
        return false;
    }

    const QString targetDir = QFileInfo(configuredPath).absolutePath();
    if (!targetDir.isEmpty()) {
        QDir().mkpath(targetDir);
    }

    if (QFileInfo::exists(configuredPath) && !QFile::remove(configuredPath)) {
        qWarning().noquote() << tr("Could not remove existing exclusion file at %1.").arg(configuredPath);
        return false;
    }

    if (!QFile::copy(sourcePath, configuredPath)) {
        qWarning().noquote()
            << tr("Could not copy default exclusion file from %1 to %2.").arg(sourcePath, configuredPath);
        return false;
    }

    return true;
}

void Batchprocessing::checkUpdatedDefaultExcludesCli()
{
    QString diffOutput;
    if (!isSourceExcludesNewer(diffOutput)) {
        return;
    }

    const QString configuredPath = settings->snapshot_excludes.fileName();
    const QString sourcePath = settings->getExcludesSourcePath();
    qDebug().noquote() << tr("Detected newer exclusion file at %1 compared to %2. Prompting for action.")
                              .arg(sourcePath, configuredPath);

    QTextStream out(stdout);
    QTextStream in(stdin);

    while (true) {
        out << tr("The exclusion file at %1 is newer than your configured file at %2.")
                   .arg(sourcePath, configuredPath)
            << '\n';
        out << tr("[s]how diff  [u]se updated default  [k]eep custom (update timestamp)  [q]uit: ") << Qt::flush;

        const QString response = in.readLine().trimmed().toLower();

        if (response == "s" || response == "show") {
            out << colorizeDiffAnsi(diffOutput) << Qt::flush;
            continue;
        }

        if (response == "u" || response == "use") {
            if (resetCustomExcludesCli(configuredPath, sourcePath)) {
                qDebug().noquote() << tr("Reverted to updated default exclusion file.");
            }
            return;
        }

        if (response == "k" || response == "keep") {
            utimbuf times {};
            times.actime = QFileInfo(configuredPath).lastRead().toSecsSinceEpoch();
            times.modtime = QDateTime::currentSecsSinceEpoch();
            const int utimeResult = utime(configuredPath.toLocal8Bit().constData(), &times);
            if (utimeResult == 0) {
                qDebug() << "Updated modification time for custom excludes file via utime" << configuredPath;
            } else {
                qWarning() << "Failed to update modification time for custom excludes file" << configuredPath;
            }
            return;
        }

        if (response == "q" || response == "quit" || response.isEmpty()) {
            qDebug() << tr("Leaving custom exclusion file unchanged.");
            const bool debugStop = qEnvironmentVariableIsSet("MX_SNAPSHOT_EXCLUDES_DEBUG_STOP");
            if (debugStop) {
                qDebug() << "Debug stop requested; exiting after excludes check.";
                QCoreApplication::exit(0);
            } else {
                QCoreApplication::exit(EXIT_SUCCESS);
            }
            return; // exit requested
        }

        out << tr("Invalid choice. Please select again.") << '\n';
    }
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
            settings->boot_options += " xorg=nvidia";
            qDebug() << tr("Note: If you use the resulting ISO on a computer without an NVIDIA card, "
                           "you will likely need to remove 'xorg=nvidia' from the boot options.");
        } else {
            qDebug() << tr("Note: If you use the resulting ISO on a computer with an NVIDIA card, "
                           "you may need to add 'xorg=nvidia' to the boot options.");
        }
    }
}
