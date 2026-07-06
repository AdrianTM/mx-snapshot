/**********************************************************************
 * test_elevationbroker.cpp
 **********************************************************************
 * Copyright (C) 2026 MX Authors
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 **********************************************************************/
#include <QCoreApplication>
#include <QFile>
#include <QTest>
#include <QTimer>

#include "elevationbroker.h"

// Exercises the broker client against a real helper running in serve mode.
// /usr/bin/env stands in for the elevation tool, so the helper runs as the
// test user and executes allow-listed commands unprivileged.
class TestElevationBroker : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        helperPath = qEnvironmentVariable("TEST_HELPER_PATH");
        if (helperPath.isEmpty()) {
            helperPath = QCoreApplication::applicationDirPath() + "/helper";
        }
        QVERIFY2(QFile::exists(helperPath), qPrintable("helper binary not found at " + helperPath));
    }

    void launchAndSimpleExec()
    {
        auto &broker = ElevationBroker::instance();
        QCOMPARE(broker.ensureStarted(helperPath, "/usr/bin/env"), ElevationBroker::Launch::Ready);
        QVERIFY(broker.isReady());

        const int code = broker.execute({"exec", "true"}, {}, nullptr, nullptr);
        QCOMPARE(code, 0);
    }

    void streamsOutputAndInput()
    {
        auto &broker = ElevationBroker::instance();
        QCOMPARE(broker.ensureStarted(helperPath, "/usr/bin/env"), ElevationBroker::Launch::Ready);

        QByteArray out;
        const int code = broker.execute({"exec", "cat"}, "hello broker\n",
                                        [&out](const QByteArray &chunk) { out += chunk; }, nullptr);
        QCOMPARE(code, 0);
        QCOMPARE(out, QByteArray("hello broker\n"));
    }

    void relaysExitCodesAndStderr()
    {
        auto &broker = ElevationBroker::instance();
        QByteArray out;
        QByteArray err;
        const int code = broker.execute({"exec", "bash", "-c", "echo out; echo err >&2; exit 3"}, {},
                                        [&out](const QByteArray &chunk) { out += chunk; },
                                        [&err](const QByteArray &chunk) { err += chunk; });
        QCOMPARE(code, 3);
        QCOMPARE(out, QByteArray("out\n"));
        QCOMPARE(err, QByteArray("err\n"));
    }

    void remapsReservedExitCodes()
    {
        auto &broker = ElevationBroker::instance();
        QByteArray err;
        const int code = broker.execute({"exec", "bash", "-c", "exit 126"}, {}, nullptr,
                                        [&err](const QByteArray &chunk) { err += chunk; });
        QCOMPARE(code, 124); // HELPER_EXIT_CHILD_REMAPPED
    }

    void rejectsDisallowedCommand()
    {
        auto &broker = ElevationBroker::instance();
        const int code = broker.execute({"exec", "definitely-not-allowed"}, {}, nullptr, nullptr);
        QCOMPARE(code, 125); // HELPER_EXIT_INTERNAL_ERROR
    }

    void killTerminatesActiveChildAndQueueContinues()
    {
        auto &broker = ElevationBroker::instance();
        // Kick off a long sleep, kill it shortly after, then verify a queued
        // request still completes.
        QTimer::singleShot(300, &broker, [&broker] { broker.killActiveChild(); });
        const int killedCode = broker.execute({"exec", "bash", "-c", "sleep 300"}, {}, nullptr, nullptr);
        QVERIFY(killedCode != 0);

        const int code = broker.execute({"exec", "true"}, {}, nullptr, nullptr);
        QCOMPARE(code, 0);
    }

    void shutdownStopsBroker()
    {
        auto &broker = ElevationBroker::instance();
        QCOMPARE(broker.ensureStarted(helperPath, "/usr/bin/env"), ElevationBroker::Launch::Ready);

        broker.shutdown();
        QVERIFY(!broker.isReady());
    }

private:
    QString helperPath;
};

QTEST_MAIN(TestElevationBroker)
#include "test_elevationbroker.moc"
