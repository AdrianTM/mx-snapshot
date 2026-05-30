/**********************************************************************
 *
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
#pragma once

#include <QDebug>
#include <QFile>
#include <QString>

class Log
{
public:
    explicit Log(const QString &fileName);
    static QString getLog();
    static void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg);
    // Safe default log location, kept out of world-writable /tmp:
    //   running as the user -> private per-user runtime dir ($XDG_RUNTIME_DIR)
    //   running as root      -> /run (root-only)
    static QString defaultLogPath(const QString &appName);

private:
    inline static QFile logFile;
    static void fixLogFileOwnership(const QString &fileName);
    // Open logFile (filename already set) for appending without following a symlink.
    static bool openLogFile();
};
