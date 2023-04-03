/**********************************************************************
 *  work.h
 **********************************************************************
 * Copyright (C) 2020 MX Authors
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

#ifndef WORK_H
#define WORK_H

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

    Work(Settings *settings, QObject *parent = nullptr);

    QElapsedTimer e_timer;
    bool done = false;
    bool started = false;

    QTemporaryDir initrd_dir;
    [[noreturn]] void cleanUp();
    bool checkAndMoveWorkDir(const QString &dir, quint64 req_size);
    bool checkInstalled(const QString &package);
    bool createIso(const QString &filename);
    bool installPackage(const QString &package);
    bool replaceStringInFile(const QString &old_text, const QString &new_text, const QString &file_path);
    quint64 getRequiredSpace();
    static void writeSnapshotInfo();
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
};

#endif // WORK_H
