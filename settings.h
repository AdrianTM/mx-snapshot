/**********************************************************************
 *  settings.h
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCommandLineParser>
#include <QFile>
#include <QTemporaryDir>

#include <cmd.h>

extern QString current_kernel;

static QHash<QString, quint8> compression_factor {{"xz", 31},  {"zstd", 35}, {"gzip", 37},
                                                  {"lzo", 52}, {"lzma", 52}, {"lz4", 52}};

class Settings
{
public:
    enum Exclude {
        Desktop = 0x1,
        Documents = 0x2,
        Downloads = 0x4,
        Music = 0x8,
        Networks = 0x16,
        Pictures = 0x32,
        Videos = 0x64,
        Steam = 0x128,
        VirtualBox = 0x256
    };
    Q_DECLARE_FLAGS(Exclusions, Exclude)

    Settings(const QCommandLineParser &arg_parser);

    Cmd *shell;
    Exclusions exclusions;
    QFile config_file;
    QFile snapshot_excludes;
    QHash<QString, QString> englishDirs; // English names of /home directories
    QScopedPointer<QTemporaryDir> tmpdir;
    QString boot_options;
    QString codename;
    QString compression;
    QString distro_version;
    QString full_distro_name;
    QString gui_editor;
    QString kernel;
    QString mksq_opt;
    QString project_name;
    QString release_date;
    QString save_message;
    QString session_excludes;
    QString snapshot_basename;
    QString snapshot_dir;
    QString snapshot_name;
    QString stamp;
    QString tempdir_parent;
    QString version;
    QString work_dir;
    QStringList users; // list of users with /home folders
    bool cli_mode;
    bool edit_boot_menu {};
    bool force_installer {};
    bool x86 {};
    bool live {};
    bool make_isohybrid {};
    bool make_md5sum {};
    bool make_sha512sum {};
    bool override_size;
    bool preempt {}; // command line option
    bool reset_accounts {};
    bool shutdown {};
    quint64 free_space {};
    quint64 free_space_work {};
    quint64 home_size {};
    quint64 root_size {};

    QString getEditor() const;
    QString getFilename() const;
    QString getFreeSpaceStrings(const QString &path);
    QString getSnapshotSize() const;
    QString getUsedSpace();
    QString getXdgUserDirs(const QString &folder);
    QString largerFreeSpace(const QString &dir1, const QString &dir2) const;
    QString largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3) const;
    QString readKernelOpts() const;
    QStringList listUsers() const;
    bool checkCompression() const;
    bool checkSnapshotDir() const;
    bool checkTempDir();
    bool isOnSupportedPart(const QString &dir) const;
    int getSnapshotCount() const;
    quint64 getFreeSpace(const QString &path) const;
    quint64 getLiveRootSpace() const;
    static bool isLive();
    static bool isi386();
    void addRemoveExclusion(bool add, QString exclusion);
    void excludeAll();
    void excludeDesktop(bool exclude);
    void excludeDocuments(bool exclude);
    void excludeDownloads(bool exclude);
    void excludeItem(const QString &item);
    void excludeMusic(bool exclude);
    void excludeNetworks(bool exclude);
    void excludePictures(bool exclude);
    void excludeSteam(bool exclude);
    void excludeSwapFile();
    void excludeVideos(bool exclude);
    void excludeVirtualBox(bool exclude);
    void loadConfig();
    void otherExclusions();
    void processArgs(const QCommandLineParser &arg_parser);
    void processExclArgs(const QCommandLineParser &arg_parser);
    void selectKernel();
    void setMonthlySnapshot(const QCommandLineParser &arg_parser);
    void setVariables();
};

#endif // SETTINGS_H
