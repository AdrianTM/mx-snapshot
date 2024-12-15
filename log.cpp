/**********************************************************************
 * Log.cpp
 **********************************************************************
 * Copyright (C) 2023-2024 MX Authors
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

Log::Log(const QString &file_name)
{
    logFile.setFileName(file_name);
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
        return;
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
