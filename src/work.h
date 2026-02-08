/**********************************************************************
 *  work.h
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

    // Main workflow methods
    [[nodiscard]] quint64 getRequiredSpace();
    void cleanUp();
    bool createIso(const QString &filename);
    void checkEnoughSpace();
    void setupEnv();
    void copyNewIso();
    void savePackageList(const QString &file_name);

    // Status accessors
    [[nodiscard]] bool isStarted() const { return started; }
    [[nodiscard]] bool isDone() const { return done; }
    [[nodiscard]] bool isCleaningUp() const { return cleanupStarted; }
    [[nodiscard]] qint64 getElapsedTime() const { return e_timer.elapsed(); }
    [[nodiscard]] const Settings& getSettings() const { return *settings; }

    // Timer control
    void startTimer() { started = true; e_timer.start(); }
    void markDone() { done = true; }

    // Utility methods
    [[nodiscard]] static bool checkInstalled(const QString &package);
    [[nodiscard]] bool isEnvironmentReady() const;
    bool installPackage(const QString &package);

    // Public members for external access
    Cmd shell;

signals:
    void message(const QString &msg);
    void messageBox(BoxType box_type, const QString &title, const QString &msg);

private:
    // Space and environment management
    [[nodiscard]] bool checkAndMoveWorkDir(const QString &dir, quint64 req_size);
    [[nodiscard]] bool checkNoSpaceAndExit(quint64 needed_space, quint64 free_space, const QString &dir);

    // Bind-root overlay management
    [[nodiscard]] bool setupBindRootOverlay();
    void cleanupBindRootOverlay();

    // File operations
    bool replaceStringInFile(const QString &old_text, const QString &new_text, const QString &file_path);

    // ISO creation helpers
    void makeChecksum(Work::HashType hash_type, const QString &folder, const QString &file_name);

    // Initrd operations
    void closeInitrd(const QString &initrd_dir, const QString &file);
    void openInitrd(const QString &file, const QString &initrd_dir);
    void copyModules(const QString &to, const QString &kernel);
    [[nodiscard]] QString kernelImageVersion(const QString &kernelPath) const;
    [[nodiscard]] QString initramfsKernelVersion(const QString &initramfsPath) const;
    bool rebuildArchisoInitramfs(const QString &archisoPath, const QString &kernelPath);

    // Configuration file generation
    void replaceMenuStrings();
    void writeLsbRelease();
    void writeSnapshotInfo();
    void writeUnsquashfsSize(const QString &text);
    void writeVersionFile();

    // Member variables
    Settings *settings;
    QElapsedTimer e_timer;
    bool started = false;
    bool done = false;
    bool cleanupStarted = false;
    QTemporaryDir initrd_dir;
    QString bindRootPath = "/.bind-root";
    QString bindRootOverlayBase;
    bool bindRootOverlayActive = false;
    QString installerLinkToRemove;  // Installer link in user's home to remove during cleanup
};
