/**********************************************************************
 *  settings.h
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

#include <QCommandLineParser>
#include <QFile>
#include <QSettings>
#include <QTemporaryDir>

#include "cmd.h"
#include "filesystemutils.h"
#include "systeminfo.h"

extern QString current_kernel;

namespace Release
{
enum Version { Jessie = 8, Stretch, Buster, Bullseye, Bookworm, Trixie, Forky, Duke };
}

class Settings
{
public:
    explicit Settings(const QCommandLineParser &arg_parser);

    [[nodiscard]] QString getEditor() const;
    [[nodiscard]] QString getFilename() const;
    [[nodiscard]] QString getFreeSpaceStrings(const QString &path);
    [[nodiscard]] QString getSnapshotSize() const;
    [[nodiscard]] QString getUsedSpace();
    [[nodiscard]] QString getXdgUserDirs(const QString &folder);
    [[nodiscard]] bool checkCompression() const;
    [[nodiscard]] bool checkConfiguration() const;
    [[nodiscard]] bool checkSnapshotDir() const;
    [[nodiscard]] bool checkTempDir();
    [[nodiscard]] bool initializeConfiguration();
    [[nodiscard]] bool validateExclusions() const;
    [[nodiscard]] bool validateSpaceRequirements() const;
    [[nodiscard]] int getSnapshotCount() const;
    [[nodiscard]] quint64 getLiveRootSpace();
    [[nodiscard]] static int getDebianVerNum();
    void addRemoveExclusion(bool add, QString exclusion);
    void excludeAll();
    void excludeDesktop(bool exclude);
    void excludeDocuments(bool exclude);
    void excludeDownloads(bool exclude);
    void excludeFlatpaks(bool exclude);
    void excludeItem(const QString &item);
    void excludeMusic(bool exclude);
    void excludeNetworks(bool exclude);
    void excludePictures(bool exclude);
    void excludeSteam(bool exclude);
    void excludeSwapFile();
    void excludeVideos(bool exclude);
    void excludeVirtualBox(bool exclude);
    void handleInitializationError(const QString &error) const;
    void loadConfig();
    void otherExclusions();
    void processArgs(const QCommandLineParser &arg_parser);
    void processExclArgs(const QCommandLineParser &arg_parser);
    void selectKernel();
    void setMonthlySnapshot(const QCommandLineParser &arg_parser);
    void setVariables();

    // Public enums and types
    enum class Exclude {
        Desktop = 1 << 0,
        Documents = 1 << 1,
        Downloads = 1 << 2,
        Flatpaks = 1 << 3,
        Music = 1 << 4,
        Networks = 1 << 5,
        Pictures = 1 << 6,
        Steam = 1 << 7,
        Videos = 1 << 8,
        VirtualBox = 1 << 9
    };
    Q_DECLARE_FLAGS(Exclusions, Exclude)

    // Phase 1: Immutable system configuration (const)
    const bool x86;
    const uint max_cores;
    const bool monthly;
    const bool override_size;
    const bool edit_boot_menu;
    const QHash<QString, quint8> compression_factor {{"xz", 31},  {"zstd", 35}, {"gzip", 37},
                                                     {"lzo", 52}, {"lzma", 52}, {"lz4", 52}};

    // Phase 2: Mutable UI preferences
    Exclusions exclusions;
    QString boot_options;
    QString codename;
    QString compression;
    QString distro_version;
    QString full_distro_name;
    QString kernel;
    QString project_name;
    QString release_date;
    bool make_md5sum {};
    bool make_sha512sum {};
    bool reset_accounts {};
    uint cores {};
    uint throttle {};

    // Phase 3: Runtime state
    QFile snapshot_excludes;
    QScopedPointer<QTemporaryDir> tmpdir;
    QString mksq_opt;
    QString session_excludes;
    QString snapshot_dir;
    QString snapshot_name;
    QString tempdir_parent;
    QString work_dir;
    bool preempt {};
    bool shutdown {};
    const bool force_installer;
    const bool live;
    const bool make_isohybrid;
    quint64 free_space {};
    quint64 free_space_work {};

private:
    QFile config_file;
    QString save_message;
    QString version;
    const QString gui_editor;
    const QString snapshot_basename;
    const QString stamp;
    const QStringList path {qEnvironmentVariable("PATH").split(":") << "/usr/sbin"};
    const QStringList users; // list of users with /home folders
    quint64 home_size {};
    quint64 root_size {};

    // Helper functions for const member initialization
    QString getInitialKernel(const QCommandLineParser &arg_parser);
    bool getEditBootMenuSetting();
    QString trimQuotes(const QString &value) const;

    struct InitialSettings {
        bool live;
        bool force_installer;
        bool make_isohybrid;
        QString gui_editor;
        QString snapshot_basename;
        QString stamp;
        QStringList users;
    };

    InitialSettings getInitialSettings() const;
};
