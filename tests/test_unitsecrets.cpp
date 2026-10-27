#include <QtTest/QtTest>

#include <QTemporaryDir>

#include "unitsecrets.h"

class TestUnitSecrets : public QObject
{
    Q_OBJECT

private slots:
    void credentialNames_data();
    void credentialNames();
    void findCredentialsScansUnitsAndDropIns();
};

void TestUnitSecrets::credentialNames_data()
{
    QTest::addColumn<QString>("unit");
    QTest::addColumn<QStringList>("expected");

    QTest::newRow("quoted assignments, like beszel-agent")
        << "[Service]\nEnvironment=\"PORT=45876\"\nEnvironment=\"KEY=ssh-ed25519 AAAA\"\n"
           "Environment=\"TOKEN=94eb\"\nEnvironment=\"HUB_URL=https://hub\"\n"
        << QStringList {"KEY", "TOKEN"};
    QTest::newRow("several unquoted assignments on one line")
        << "Environment=DB_PASSWORD=x API_KEY=y LOG_LEVEL=info\n" << QStringList {"DB_PASSWORD", "API_KEY"};
    QTest::newRow("single-quoted, mixed case, suffixes")
        << "Environment='client_secret=a' 'SSH_PRIVATE_KEY=b' 'GITHUB_TOKENS=c'\n"
        << QStringList {"client_secret", "SSH_PRIVATE_KEY", "GITHUB_TOKENS"};
    QTest::newRow("commented-out line") << "# Environment=\"TOKEN=abc\"\n;Environment=TOKEN=abc\n" << QStringList {};
    QTest::newRow("empty value") << "Environment=\"TOKEN=\"\nEnvironment=PASSWORD=\n" << QStringList {};
    QTest::newRow("file paths and harmless names")
        << "Environment=TOKEN_FILE=/run/t KEYMAP=us KEYBOARD=pc105 MONKEY=1\n" << QStringList {};
    QTest::newRow("not an Environment line")
        << "ExecStart=/usr/bin/agent --token=abc\nEnvironmentFile=/etc/agent.env\n" << QStringList {};
    QTest::newRow("same name twice") << "Environment=TOKEN=a\nEnvironment=TOKEN=b\n" << QStringList {"TOKEN"};
    QTest::newRow("continuation line")
        << "[Service]\nEnvironment=PORT=80 \\\n  TOKEN=fixture\n" << QStringList {"TOKEN"};
    QTest::newRow("comment inside a continuation")
        << "Environment=PORT=80 \\\n# a comment\n  API_KEY=fixture\n" << QStringList {"API_KEY"};
    QTest::newRow("comment ending in a backslash")
        << "[Service]\n# old setting \\\nEnvironment=TOKEN=fixture\n" << QStringList {"TOKEN"};
    QTest::newRow("assignment-like text inside a quoted value")
        << "Environment=\"MESSAGE=hello TOKEN=fixture\" 'NOTE=PASSWORD=x'\n" << QStringList {};
    QTest::newRow("quoted word with a space in the value")
        << "Environment=\"SECRET=two words\" PORT=1\n" << QStringList {"SECRET"};
}

void TestUnitSecrets::credentialNames()
{
    QFETCH(QString, unit);
    QFETCH(QStringList, expected);
    QCOMPARE(UnitSecrets::credentialNames(unit), expected);
}

void TestUnitSecrets::findCredentialsScansUnitsAndDropIns()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const auto write = [&dir](const QString &relative, const QByteArray &text) {
        const QString path = dir.filePath(relative);
        QVERIFY(QDir().mkpath(QFileInfo(path).absolutePath()));
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(text);
    };
    write("agent.service", "[Service]\nEnvironment=\"TOKEN=abc\"\n");
    write("plain.service", "[Service]\nExecStart=/bin/true\n");
    write("web.service.d/override.conf", "[Service]\nEnvironment=API_KEY=xyz\n");
    // Not unit files or drop-ins: nested enable links and stray files.
    write("multi-user.target.wants/notes.conf", "Environment=TOKEN=abc\n");
    write("web.service.d/deeper/x.conf", "Environment=TOKEN=abc\n");
    write("web.service.d/readme.txt", "Environment=TOKEN=abc\n");
    // Enable links point at units scanned on their own.
    QVERIFY(QFile::link(dir.filePath("agent.service"), dir.filePath("alias.service")));
    // Unreadable files are reported, not passed as clean (root ignores modes).
    write("private.service", "[Service]\nEnvironment=TOKEN=abc\n");
    QVERIFY(QFile::setPermissions(dir.filePath("private.service"), QFileDevice::WriteOwner));
    const bool runningAsRoot = QFile(dir.filePath("private.service")).open(QIODevice::ReadOnly);
    // So are drop-in directories that can't be listed.
    write("locked.service.d/override.conf", "[Service]\nEnvironment=TOKEN=abc\n");
    QVERIFY(QFile::setPermissions(dir.filePath("locked.service.d"), QFileDevice::WriteOwner));

    const UnitSecrets::ScanResult scan = UnitSecrets::findCredentials(dir.path());
    QStringList expectedFindings {"agent.service: TOKEN", "web.service.d/override.conf: API_KEY"};
    QStringList expectedUnreadable {"locked.service.d/", "private.service"};
    if (runningAsRoot) {
        expectedFindings = QStringList {"agent.service: TOKEN", "locked.service.d/override.conf: TOKEN",
                                        "private.service: TOKEN", "web.service.d/override.conf: API_KEY"};
        expectedUnreadable.clear();
    }
    // Restore before comparing, so a failure still leaves QTemporaryDir removable.
    QVERIFY(QFile::setPermissions(dir.filePath("locked.service.d"),
                                  QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
    QCOMPARE(scan.findings, expectedFindings);
    QCOMPARE(scan.unreadable, expectedUnreadable);

    // An unlistable unit directory is reported as a whole.
    QVERIFY(QFile::setPermissions(dir.path(), QFileDevice::WriteOwner));
    const UnitSecrets::ScanResult locked = UnitSecrets::findCredentials(dir.path());
    QVERIFY(QFile::setPermissions(dir.path(),
                                  QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
    if (!runningAsRoot) {
        QVERIFY(locked.findings.isEmpty());
        QCOMPARE(locked.unreadable, QStringList {QDir::cleanPath(dir.path()) + '/'});
    }

    const UnitSecrets::ScanResult missing = UnitSecrets::findCredentials(dir.filePath("missing"));
    QVERIFY(missing.findings.isEmpty());
    QVERIFY(missing.unreadable.isEmpty());
}

QTEST_GUILESS_MAIN(TestUnitSecrets)
#include "test_unitsecrets.moc"
