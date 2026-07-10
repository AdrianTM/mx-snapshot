#include "filesystemutils.h"

#include <QtCore/QDebug>
#include <QtCore/QStorageInfo>
const QSet<QString> FileSystemUtils::supportedPartitions = {
    "ext2", "ext3", "ext4", "btrfs", "xfs", "f2fs", "jfs", "tmpfs",
    "zfs", "bcachefs", "nilfs2", "reiserfs",
    // Live systems: snapshots are routinely taken from a running live session,
    // where / (and thus /tmp or /home) sits on an overlay.
    "overlay", "aufs",
};

quint64 FileSystemUtils::getFreeSpace(const QString &path)
{
    QStorageInfo storage(path + "/");
    if (!storage.isReady() || storage.isReadOnly()) {
        qDebug() << "Cannot determine free space for" << path
                 << ": Drive not ready, or does not exist, or is read-only.";
        return 0;
    }
    return storage.bytesAvailable() / 1024;
}

QString FileSystemUtils::getFreeSpaceString(const QString &path)
{
    constexpr float factor = 1024 * 1024;
    const quint64 freeSpace = getFreeSpace(path);
    return QString::number(static_cast<double>(freeSpace) / factor, 'f', 2) + "GiB";
}

bool FileSystemUtils::isOnSupportedPartition(const QString &dir)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    const QStorageInfo storage(dir + "/");
    if (!storage.isValid() || !storage.isReady() || storage.isReadOnly()) {
        qDebug() << "Rejecting" << dir << ": storage not valid, not ready, or read-only";
        return false;
    }
    const QString partType = storage.fileSystemType();
    const bool isSupported = isSupportedFilesystemType(partType);
    qDebug() << "Detected partition:" << partType << "Supported part:" << isSupported;
    return isSupported;
}

bool FileSystemUtils::isSupportedFilesystemType(const QString &type)
{
    return supportedPartitions.contains(type);
}

QString FileSystemUtils::largerFreeSpace(const QString &dir1, const QString &dir2)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (QStorageInfo(dir1 + "/").device() == QStorageInfo(dir2 + "/").device()) {
        return dir1;
    }
    const quint64 dir1Free = getFreeSpace(dir1);
    const quint64 dir2Free = getFreeSpace(dir2);
    return dir1Free >= dir2Free ? dir1 : dir2;
}

QString FileSystemUtils::largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    return largerFreeSpace(largerFreeSpace(dir1, dir2), dir3);
}
