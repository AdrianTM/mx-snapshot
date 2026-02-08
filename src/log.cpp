/**********************************************************************
 * Log.cpp
 **********************************************************************
 * Copyright (C) 2023-2025 MX Authors
 *
 * Authors: Adrian <adrian@mxlinux.org>
 *          MX Linux <http://mxlinux.org>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package. If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/
#include "log.h"

#include <QDateTime>
#include <QFileInfo>

#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>

#include "cmd.h"

Log::Log(const QString &fileName)
{
    logFile.setFileName(fileName);

    // Check if log file exists and has wrong ownership
    if (QFileInfo::exists(fileName)) {
        fixLogFileOwnership(fileName);
    }

    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Could not open log file:" << fileName;
    } else {
        qInstallMessageHandler(Log::messageHandler);
    }
}

void Log::messageHandler(QtMsgType type, [[maybe_unused]] const QMessageLogContext &context, const QString &msg)
{
    static thread_local bool inHandler = false;
    if (inHandler) {
        std::fputs((msg + '\n').toLocal8Bit().constData(), stderr);
        return;
    }
    inHandler = true;

    QTextStream termOut(stdout);

    // Check if the message contains carriage return or starts with the escape sequence for clearing the line
    if (msg.contains('\r') || msg.startsWith("\033[2K")) {
        termOut << "\033[?25l" << msg;
        inHandler = false;
        return; // Skip writing to the log file
    }

    termOut << msg << '\n';

    if (!logFile.isOpen()) {
        std::fputs(
            ("Log file is not open for writing: " + logFile.fileName() + '\n').toLocal8Bit().constData(), stderr);
        // Try to fix ownership and reopen
        fixLogFileOwnership(logFile.fileName());
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            std::fputs("Still could not open log file after ownership fix\n", stderr);
            inHandler = false;
            return;
        }
    }

    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type) {
    case QtInfoMsg:
        out << "INF";
        break;
    case QtDebugMsg:
        out << "DBG";
        break;
    case QtWarningMsg:
        out << "WRN";
        break;
    case QtCriticalMsg:
        out << "CRT";
        break;
    case QtFatalMsg:
        out << "FTL";
        break;
    }

    out << ": " << msg << '\n';
    inHandler = false;
}

QString Log::getLog()
{
    return logFile.fileName();
}

void Log::fixLogFileOwnership(const QString &fileName)
{
    const QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) {
        return;
    }

    // Get current user information
    const uid_t currentUid = getuid();

    // Get file ownership and permissions
    struct stat fileStat;
    const QByteArray fileNameBytes = fileName.toLocal8Bit();
    if (stat(fileNameBytes.constData(), &fileStat) != 0) {
        return;
    }

    // Case 1: Running as regular user, but file is owned by root
    if (fileStat.st_uid == 0 && currentUid != 0) {
        Cmd cmd;
        cmd.runAsRoot("chown $(logname): \"" + fileName + "\"", Cmd::QuietMode::Yes);
    }
    // Case 2: Running as root, but file is owned by regular user with restrictive permissions
    else if (fileStat.st_uid != 0 && currentUid == 0) {
        // When running as root, take ownership of the log file for consistency
        if (chown(fileNameBytes.constData(), 0, 0) == 0) {
            // Also ensure it has reasonable permissions
            chmod(fileNameBytes.constData(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
    }
}
