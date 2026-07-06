#include <QtTest/QtTest>

#include "cmd.h"

class TestCmd : public QObject
{
    Q_OBJECT

private slots:
    void coreApplicationIsCliMode();
};

void TestCmd::coreApplicationIsCliMode()
{
    QVERIFY(Cmd::isCliMode());
}

QTEST_GUILESS_MAIN(TestCmd)
#include "test_cmd.moc"
