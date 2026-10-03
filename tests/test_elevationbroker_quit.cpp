/**********************************************************************
 * test_elevationbroker_quit.cpp
 **********************************************************************
 * Copyright (C) 2026-2026 MX Authors
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 **********************************************************************/
#include <QCoreApplication>
#include <QTest>
#include <QTimer>

#include "elevationbroker.h"

// Quitting while the broker waits for authentication. Kept apart from
// test_elevationbroker: a quit shuts the process-wide broker down for good.
class TestElevationBrokerQuit : public QObject
{
    Q_OBJECT

private slots:
    void quitDuringLaunchAborts()
    {
        auto &broker = ElevationBroker::instance();
        // `cat - serve` stands in for pkexec showing its dialog: it reads the
        // still-open stdin until killed and never prints READY. exit(0), as
        // sent by QApplication's quit, must end the wait like any exit code.
        QTimer::singleShot(100, qApp, [] { QCoreApplication::exit(0); });
        QCOMPARE(broker.ensureStarted("-", "cat"), ElevationBroker::Launch::Aborted);
        QVERIFY(!broker.isReady());
    }

    void laterLaunchesStayAborted()
    {
        // If it were launched, `sh -c serve` would exit 127 and be reported as
        // Denied. Nothing may be started (or prompted for) while quitting.
        QCOMPARE(ElevationBroker::instance().ensureStarted("-c", "sh"), ElevationBroker::Launch::Aborted);
    }
};

QTEST_MAIN(TestElevationBrokerQuit)
#include "test_elevationbroker_quit.moc"
