/**********************************************************************
 *  bindrootmanager.h
 **********************************************************************
 * Copyright (C) 2025 MX Authors
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

#include <QString>
#include <QStringList>

#include "cmd.h"

class BindRootManager
{
public:
    BindRootManager(Cmd &shell, const QString &bindRoot, const QString &workDir);

    [[nodiscard]] bool start(bool force);
    [[nodiscard]] bool makeReadOnly();
    [[nodiscard]] bool doLiveFiles();
    [[nodiscard]] bool doGeneralFiles();
    [[nodiscard]] bool doGeneral();
    [[nodiscard]] bool doVersionFile(const QString &title = QString());
    [[nodiscard]] bool doAdjtime();
    [[nodiscard]] bool doBindMounts(const QStringList &dirs);
    [[nodiscard]] bool doEmptyDirs(const QStringList &dirs);
    [[nodiscard]] bool doPasswd();
    [[nodiscard]] bool cleanup();
    void addRmFile(const QString &path);

    [[nodiscard]] static bool hasCleanupState();

private:
    [[nodiscard]] QString stateFilePath();
    [[nodiscard]] QString stateDirPath();
    [[nodiscard]] QString fallbackStateDirPath() const;
    [[nodiscard]] bool loadState();
    [[nodiscard]] bool persistState();
    [[nodiscard]] bool ensureStateDir();
    void addRmDir(const QString &path);

    [[nodiscard]] bool bindMountTemplate(const QString &templateDir);
    [[nodiscard]] bool makeDir(const QString &targetDir, const QString &origDir);
    [[nodiscard]] bool touchFile(const QString &targetFile, const QString &origFile);
    [[nodiscard]] bool copyTemplateDir(const QString &source, const QString &dest);

    [[nodiscard]] bool doRepo();
    [[nodiscard]] bool doTimezone();
    [[nodiscard]] QString readFileRoot(const QString &path);
    [[nodiscard]] bool writeFileRoot(const QString &path, const QString &content);
    [[nodiscard]] bool statFileRoot(const QString &path, QString &mode, QString &uid, QString &gid);

    Cmd &shell;
    QString bindRoot;
    QString workDir;
    QString realRoot = "/";
    QStringList rmDirs;
    QStringList rmFiles;
    bool stateLoaded = false;
};
