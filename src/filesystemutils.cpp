#include "filesystemutils.h"

#include <QtCore/QDebug>
#include <QtCore/QStorageInfo>
const QSet<QString> FileSystemUtils::unsupportedPartitions = {
    "fat", "vfat", "msdos", "exfat", "ntfs", "ntfs3", "ntfs-3g"
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
    const QString partType = QStorageInfo(dir + "/").fileSystemType();
    const bool isUnsupported = unsupportedPartitions.contains(partType);
    const bool isSupported = !isUnsupported;
    qDebug() << "Detected partition:" << partType << "Supported part:" << isSupported;
    return isSupported;
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
