/**********************************************************************
 *  excludesutils.cpp
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

#include "excludesutils.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

#include <utime.h>

namespace ExcludesUtils
{

bool isSourceExcludesNewer(const QString &configuredPath, const QString &sourcePath, QString &diffOutput)
{
    if (sourcePath.isEmpty() || configuredPath.isEmpty()) {
        return false;
    }

    const QFileInfo configuredInfo(configuredPath);
    const QFileInfo sourceInfo(sourcePath);

    if (!configuredInfo.exists() || !sourceInfo.exists()) {
        return false;
    }

    if (sourceInfo.lastModified() <= configuredInfo.lastModified()) {
        return false;
    }

    QProcess diffProcess;
    diffProcess.start("diff", {"--unified", configuredPath, sourcePath});
    if (!diffProcess.waitForFinished()) {
        qWarning() << "Unable to compare excludes files with diff:" << configuredPath << sourcePath;
        return false;
    }

    const int diffResult = diffProcess.exitCode();
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
        diffOutput = QObject::tr("No diff output available.");
    }
    return true;
}

bool resetCustomExcludes(const QString &configuredPath, const QString &sourcePath)
{
    if (sourcePath.isEmpty() || configuredPath.isEmpty()) {
        return false;
    }

    if (!QFileInfo::exists(sourcePath)) {
        qWarning().noquote() << QObject::tr("Default exclusion file not found at %1.").arg(sourcePath);
        return false;
    }

    const QString targetDir = QFileInfo(configuredPath).absolutePath();
    if (!targetDir.isEmpty()) {
        QDir().mkpath(targetDir);
    }

    if (QFileInfo::exists(configuredPath)) {
        const QString backupPath = configuredPath + "." + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        if (!QFile::copy(configuredPath, backupPath)) {
            qWarning().noquote() << QObject::tr("Could not backup existing exclusion file to %1.").arg(backupPath);
        }
        if (!QFile::remove(configuredPath)) {
            qWarning().noquote()
                << QObject::tr("Could not remove existing exclusion file at %1.").arg(configuredPath);
            return false;
        }
    }

    if (!QFile::copy(sourcePath, configuredPath)) {
        qWarning().noquote()
            << QObject::tr("Could not copy default exclusion file from %1 to %2.").arg(sourcePath, configuredPath);
        return false;
    }

    return true;
}

bool touchExcludesTimestamp(const QString &configuredPath)
{
    utimbuf times {};
    times.actime = QFileInfo(configuredPath).lastRead().toSecsSinceEpoch();
    times.modtime = QDateTime::currentSecsSinceEpoch();
    const int utimeResult = utime(configuredPath.toLocal8Bit().constData(), &times);
    if (utimeResult == 0) {
        qDebug() << "Updated modification time for custom excludes file via utime" << configuredPath;
        return true;
    }
    qWarning() << "Failed to update modification time for custom excludes file" << configuredPath;
    return false;
}

bool hasNvidiaGraphicsCard(Cmd &shell)
{
    return shell.run("glxinfo | grep -q NVIDIA");
}

} // namespace ExcludesUtils
