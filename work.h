/**********************************************************************
 *  work.h
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
#pragma once

#include <QElapsedTimer>

#include "cmd.h"
#include "settings.h"

enum class BoxType { question, information, warning, critical };

class Work : public QObject
{
    Q_OBJECT
public:
    enum HashType { md5, sha512 };
    Q_ENUM(HashType)

    explicit Work(Settings *settings, QObject *parent = nullptr);
    friend class Settings;
    friend class Batchprocessing;
    friend class MainWindow;

    [[noreturn]] void cleanUp();
    bool checkAndMoveWorkDir(const QString &dir, quint64 req_size);
    static bool checkInstalled(const QString &package);
    bool createIso(const QString &filename);
    bool installPackage(const QString &package);
    bool replaceStringInFile(const QString &old_text, const QString &new_text, const QString &file_path);
    quint64 getRequiredSpace();
    void writeSnapshotInfo();
    void checkEnoughSpace();
    void checkNoSpaceAndExit(quint64 needed_space, quint64 free_space, const QString &dir);
    void closeInitrd(const QString &initrd_dir, const QString &file);
    void copyModules(const QString &to, const QString &kernel);
    void copyNewIso();
    void makeChecksum(Work::HashType hash_type, const QString &folder, const QString &file_name);
    void openInitrd(const QString &file, const QString &initrd_dir);
    void replaceMenuStrings();
    void savePackageList(const QString &file_name);
    void setupEnv();
    void writeLsbRelease();
    void writeUnsquashfsSize(const QString &text);
    void writeVersionFile();

signals:
    void message(const QString &msg);
    void messageBox(BoxType box_type, const QString &title, const QString &msg);

private:
    Settings *settings;
    QString elevate {QFile::exists("/usr/bin/pkexec") ? "/usr/bin/pkexec" : "/usr/bin/gksu"};
    Cmd shell;
    QElapsedTimer e_timer;
    QTemporaryDir initrd_dir;
    bool done = false;
    bool started = false;
};
