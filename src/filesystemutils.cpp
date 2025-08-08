#include "filesystemutils.h"

#include <QtCore/QDebug>
#include <QtCore/QStorageInfo>

const QSet<QString> FileSystemUtils::supportedPartitions = {
    "ext2", "ext3", "ext4", "btrfs", "jfs", "xfs", "overlay", "fuseblk", "ramfs", "tmpfs", "zfs"
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
    quint64 free_space = getFreeSpace(path);
    return QString::number(static_cast<double>(free_space) / factor, 'f', 2) + "GiB";
}

bool FileSystemUtils::isOnSupportedPartition(const QString &dir)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    const QString partType = QStorageInfo(dir + "/").fileSystemType();
    const bool isSupported = supportedPartitions.contains(partType);
    qDebug() << "Detected partition:" << partType << "Supported part:" << isSupported;
    return isSupported;
}

QString FileSystemUtils::largerFreeSpace(const QString &dir1, const QString &dir2)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    if (QStorageInfo(dir1 + "/").device() == QStorageInfo(dir2 + "/").device()) {
        return dir1;
    }
    quint64 dir1_free = getFreeSpace(dir1);
    quint64 dir2_free = getFreeSpace(dir2);
    return dir1_free >= dir2_free ? dir1 : dir2;
}

QString FileSystemUtils::largerFreeSpace(const QString &dir1, const QString &dir2, const QString &dir3)
{
    qDebug() << "+++" << __PRETTY_FUNCTION__ << "+++";
    return largerFreeSpace(largerFreeSpace(dir1, dir2), dir3);
}