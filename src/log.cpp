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

#include <unistd.h>
#include <sys/stat.h>

#include "cmd.h"

Log::Log(const QString &file_name)
{
    logFile.setFileName(file_name);

    // Check if log file exists and has wrong ownership
    if (QFileInfo::exists(file_name)) {
        fixLogFileOwnership(file_name);
    }

    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Could not open log file:" << file_name;
    } else {
        qInstallMessageHandler(Log::messageHandler);
    }
}

void Log::messageHandler(QtMsgType type, [[maybe_unused]] const QMessageLogContext &context, const QString &msg)
{
    QTextStream term_out(stdout);

    // Check if the message contains carriage return or starts with the escape sequence for clearing the line
    if (msg.contains('\r') || msg.startsWith("\033[2K")) {
        term_out << "\033[?25l" << msg;
        return; // Skip writing to the log file
    }

    term_out << msg << '\n';

    if (!logFile.isOpen()) {
        qWarning() << "Log file is not open for writing:" << logFile.fileName();
        // Try to fix ownership and reopen
        fixLogFileOwnership(logFile.fileName());
        if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Still could not open log file after ownership fix";
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
}

QString Log::getLog()
{
    return logFile.fileName();
}

void Log::fixLogFileOwnership(const QString &file_name)
{
    QFileInfo fileInfo(file_name);
    if (!fileInfo.exists()) {
        return;
    }

    // Get current user information
    uid_t currentUid = getuid();

    // Get file ownership and permissions
    struct stat fileStat;
    if (stat(file_name.toLocal8Bit().constData(), &fileStat) != 0) {
        return;
    }

    // Case 1: Running as regular user, but file is owned by root
    if (fileStat.st_uid == 0 && currentUid != 0) {
        Cmd cmd;
        if (cmd.runAsRoot("chown $(logname): \"" + file_name + "\"", true)) {
            qDebug() << "Fixed log file ownership for:" << file_name;
        }
    }
    // Case 2: Running as root, but file is owned by regular user with restrictive permissions
    else if (fileStat.st_uid != 0 && currentUid == 0) {
        // When running as root, take ownership of the log file for consistency
        if (chown(file_name.toLocal8Bit().constData(), 0, 0) == 0) {
            qDebug() << "Took ownership of log file as root:" << file_name;
            // Also ensure it has reasonable permissions
            chmod(file_name.toLocal8Bit().constData(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
    }
}
