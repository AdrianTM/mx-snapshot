/**********************************************************************
 * test_checksumutils.cpp
 **********************************************************************
 * Copyright (C) 2026 MX Authors
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 **********************************************************************/
#include <QCryptographicHash>
#include <QProcess>
#include <QTemporaryDir>
#include <QtTest/QtTest>

#include "checksumutils.h"

// Executes the checksum scripts the same way Work::makeChecksum does
// (bash -c with the file name and folder as positional parameters), including
// with failing command stubs on PATH, and asserts that a failure is visible to
// the caller: non-zero exit status and no valid checksum file. This is the
// regression guard for reporting a snapshot successful when a requested
// checksum could not be produced.
class TestChecksumUtils : public QObject
{
    Q_OBJECT

private slots:
    void directCommandCreatesChecksum();
    void directCommandFailurePropagates();
    void preemptCommandCreatesChecksum();
    void preemptCommandFailurePropagates();
    void preemptCommandFailsWhenSourceMissing();
    void preemptCommandFailsWhenTempDirMissing();
    void verifyRejectsMissingOrEmptyFile();

private:
    // Mirrors Work::makeChecksum's invocation: script via -c, "_" as $0,
    // file name as $1 and destination folder as $2, run from within folder.
    static int runScript(const QString &script, const QString &fileName, const QString &folder,
                         const QString &pathPrepend = {})
    {
        QProcess proc;
        proc.setWorkingDirectory(folder);
        if (!pathPrepend.isEmpty()) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("PATH", pathPrepend + ":" + env.value("PATH"));
            proc.setProcessEnvironment(env);
        }
        proc.start("/bin/bash", {"-c", script, "_", fileName, folder});
        if (!proc.waitForFinished(10000) || proc.exitStatus() != QProcess::NormalExit) {
            return -1;
        }
        return proc.exitCode();
    }

    static QString writeSourceFile(const QString &folder, const QString &fileName, const QByteArray &content)
    {
        QFile file(folder + "/" + fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            return {};
        }
        file.write(content);
        file.close();
        return file.fileName();
    }

    // A stub md5sum that always fails, standing in for a checksum tool error.
    static QString makeFailingStubDir(const QTemporaryDir &base)
    {
        const QString stubDir = base.path() + "/stubs";
        QDir().mkpath(stubDir);
        QFile stub(stubDir + "/md5sum");
        if (!stub.open(QIODevice::WriteOnly)) {
            return {};
        }
        stub.write("#!/bin/sh\nexit 1\n");
        stub.close();
        stub.setPermissions(stub.permissions() | QFileDevice::ExeOwner | QFileDevice::ExeGroup
                            | QFileDevice::ExeOther);
        return stubDir;
    }
};

void TestChecksumUtils::directCommandCreatesChecksum()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QByteArray content = "checksum me\n";
    QVERIFY(!writeSourceFile(dir.path(), "test.iso", content).isEmpty());

    QCOMPARE(runScript(ChecksumUtils::directCommand("md5"), "test.iso", dir.path()), 0);
    QVERIFY(ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));

    QFile checksumFile(ChecksumUtils::checksumFilePath(dir.path(), "test.iso", "md5"));
    QVERIFY(checksumFile.open(QIODevice::ReadOnly));
    const QString expected = QCryptographicHash::hash(content, QCryptographicHash::Md5).toHex();
    QCOMPARE(QString(checksumFile.readAll()).section(' ', 0, 0), expected);
}

void TestChecksumUtils::directCommandFailurePropagates()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(!writeSourceFile(dir.path(), "test.iso", "data\n").isEmpty());
    const QString stubDir = makeFailingStubDir(dir);
    QVERIFY(!stubDir.isEmpty());

    QVERIFY(runScript(ChecksumUtils::directCommand("md5"), "test.iso", dir.path(), stubDir) != 0);
    // The redirect leaves an empty file behind; it must not count as a checksum.
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));
}

void TestChecksumUtils::preemptCommandCreatesChecksum()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QByteArray content = "checksum me too\n";
    QVERIFY(!writeSourceFile(dir.path(), "test.iso", content).isEmpty());
    // The caller (Work::makeChecksum) creates and owns the private temp dir.
    const QString tempDir = dir.path() + "/checksum-temp";
    QVERIFY(QDir().mkpath(tempDir));

    QCOMPARE(runScript(ChecksumUtils::preemptCommand("md5", tempDir), "test.iso", dir.path()), 0);
    QVERIFY(ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));
    // Removal is the caller's job; the script must never delete the directory.
    QVERIFY(QFileInfo::exists(tempDir));

    QFile checksumFile(ChecksumUtils::checksumFilePath(dir.path(), "test.iso", "md5"));
    QVERIFY(checksumFile.open(QIODevice::ReadOnly));
    const QString expected = QCryptographicHash::hash(content, QCryptographicHash::Md5).toHex();
    QCOMPARE(QString(checksumFile.readAll()).section(' ', 0, 0), expected);
}

void TestChecksumUtils::preemptCommandFailurePropagates()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(!writeSourceFile(dir.path(), "test.iso", "data\n").isEmpty());
    const QString stubDir = makeFailingStubDir(dir);
    QVERIFY(!stubDir.isEmpty());
    const QString tempDir = dir.path() + "/checksum-temp";
    QVERIFY(QDir().mkpath(tempDir));

    QVERIFY(runScript(ChecksumUtils::preemptCommand("md5", tempDir), "test.iso", dir.path(), stubDir) != 0);
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));
}

void TestChecksumUtils::preemptCommandFailsWhenSourceMissing()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString tempDir = dir.path() + "/checksum-temp";
    QVERIFY(QDir().mkpath(tempDir));

    // cp of the missing source must fail the whole script (set -e), not be
    // masked by a later command succeeding.
    QVERIFY(runScript(ChecksumUtils::preemptCommand("md5", tempDir), "no-such-file.iso", dir.path()) != 0);
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "no-such-file.iso", "md5"));
}

void TestChecksumUtils::preemptCommandFailsWhenTempDirMissing()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(!writeSourceFile(dir.path(), "test.iso", "data\n").isEmpty());

    // The script must not create the temp dir itself (that would reopen the
    // door to a pre-created attacker path being acceptable); a missing dir is
    // a hard failure.
    const QString tempDir = dir.path() + "/does-not-exist";
    QVERIFY(runScript(ChecksumUtils::preemptCommand("md5", tempDir), "test.iso", dir.path()) != 0);
    QVERIFY(!QFileInfo::exists(tempDir));
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));
}

void TestChecksumUtils::verifyRejectsMissingOrEmptyFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));

    // An empty checksum file (failed command after the redirect opened it).
    QVERIFY(!writeSourceFile(dir.path(), "test.iso.md5", {}).isEmpty());
    QVERIFY(!ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));

    QVERIFY(!writeSourceFile(dir.path(), "test.iso.md5", "d41d8cd98f00b204e9800998ecf8427e  test.iso\n").isEmpty());
    QVERIFY(ChecksumUtils::verifyChecksumFile(dir.path(), "test.iso", "md5"));
}

QTEST_GUILESS_MAIN(TestChecksumUtils)
#include "test_checksumutils.moc"
