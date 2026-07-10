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

    // Check if directory is on a partition the snapshot work dir can live on:
    // valid, ready, writable, and a filesystem type from the allowlist below.
    [[nodiscard]] static bool isOnSupportedPartition(const QString &dir);

    // Pure classifier behind isOnSupportedPartition(): true only for local
    // POSIX filesystems known to handle the work-dir load (multi-GB files,
    // symlinks, ownership). Everything else — vfat/ntfs, network mounts, any
    // fuse.* type, VM shared folders (vmhgfs/vboxsf/virtiofs), and unknown
    // types — is rejected; callers fall back to another candidate directory.
    [[nodiscard]] static bool isSupportedFilesystemType(const QString &type);

    // Return directory with larger free space between two options
    [[nodiscard]] static QString largerFreeSpace(const QString &dir1, const QString &dir2);

    // Return directory with largest free space among three options
    [[nodiscard]] static QString largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3);

private:
    // Allowlist of filesystems exercised with the snapshot workflow. A wrong
    // rejection here just moves the work dir to /tmp or /home; a wrong
    // acceptance surfaces as an hours-late mksquashfs/xorriso failure.
    static const QSet<QString> supportedPartitions;
};
