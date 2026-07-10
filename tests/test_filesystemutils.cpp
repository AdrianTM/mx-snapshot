/**********************************************************************
 * test_filesystemutils.cpp
 **********************************************************************
 * Copyright (C) 2026 MX Authors
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 **********************************************************************/
#include <QtTest/QtTest>

#include "filesystemutils.h"

// Guards the work-dir placement classifier: only local POSIX filesystems may
// be accepted, and everything unknown, remote, fuse-backed, or unreadable must
// be rejected so the fallback chain picks a safe candidate instead of failing
// hours later inside mksquashfs/xorriso.
class TestFileSystemUtils : public QObject
{
    Q_OBJECT

private slots:
    void acceptsLocalPosixFilesystems_data()
    {
        QTest::addColumn<QString>("type");
        for (const char *type : {"ext2", "ext3", "ext4", "btrfs", "xfs", "f2fs", "jfs", "tmpfs", "zfs", "overlay"}) {
            QTest::newRow(type) << QString(type);
        }
    }

    void acceptsLocalPosixFilesystems()
    {
        QFETCH(QString, type);
        QVERIFY(FileSystemUtils::isSupportedFilesystemType(type));
    }

    void rejectsNonPosixRemoteAndUnknownTypes_data()
    {
        QTest::addColumn<QString>("type");
        // No POSIX permissions / no large files
        for (const char *type : {"vfat", "exfat", "ntfs", "ntfs3", "msdos"}) {
            QTest::newRow(type) << QString(type);
        }
        // Network filesystems -- including nfs, which the old denylist missed
        for (const char *type : {"nfs", "nfs4", "cifs", "smbfs", "9p", "glusterfs"}) {
            QTest::newRow(type) << QString(type);
        }
        // fuse.* is an open set; any spelling must be rejected without enumeration
        for (const char *type : {"fuse", "fuseblk", "fuse.sshfs", "fuse.rclone", "fuse.vmhgfs-fuse"}) {
            QTest::newRow(type) << QString(type);
        }
        // VM host-shared folders (VMware HGFS, VirtualBox, virtiofs)
        for (const char *type : {"vmhgfs", "vmhgfs-fuse", "vboxsf", "virtiofs"}) {
            QTest::newRow(type) << QString(type);
        }
        // Unknown or empty (invalid QStorageInfo reports an empty type)
        QTest::newRow("empty") << QString();
        QTest::newRow("unknown") << QStringLiteral("somefs");
    }

    void rejectsNonPosixRemoteAndUnknownTypes()
    {
        QFETCH(QString, type);
        QVERIFY(!FileSystemUtils::isSupportedFilesystemType(type));
    }

    void rejectsNonexistentPath()
    {
        QVERIFY(!FileSystemUtils::isOnSupportedPartition(QStringLiteral("/nonexistent/path/for/test")));
    }
};

QTEST_GUILESS_MAIN(TestFileSystemUtils)
#include "test_filesystemutils.moc"
