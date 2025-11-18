#pragma once

#include <QtCore/QString>
#include <QtCore/QSet>

class FileSystemUtils
{
public:
    // Get free space in KiB for a given path
    [[nodiscard]] static quint64 getFreeSpace(const QString &path);

    // Get formatted free space string for display
    [[nodiscard]] static QString getFreeSpaceString(const QString &path);

    // Check if directory is on a supported partition type
    [[nodiscard]] static bool isOnSupportedPartition(const QString &dir);

    // Return directory with larger free space between two options
    [[nodiscard]] static QString largerFreeSpace(const QString &dir1, const QString &dir2);

    // Return directory with largest free space among three options
    [[nodiscard]] static QString largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3);

private:
    // Filesystems lacking POSIX permissions/ownership support
    static const QSet<QString> unsupportedPartitions;
};
