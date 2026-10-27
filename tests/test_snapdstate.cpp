#include <QtTest/QtTest>

#include <QJsonDocument>
#include <QJsonObject>

#include "snapdstate.h"

class TestSnapdState : public QObject
{
    Q_OBJECT

private slots:
    void removesDeviceIdentity();
    void removesUsersWithoutDevice();
    void keepsStateWithoutDevice();
    void rejectsNonState_data();
    void rejectsNonState();
};

void TestSnapdState::removesDeviceIdentity()
{
    const QByteArray state = R"({"data":{
        "auth":{"last-id":1,"macaroon-key":"Zml4dHVyZQ==",
                "users":[{"id":1,"username":"fixture","email":"fixture@example.com","macaroon":"m",
                          "discharges":["d"],"store-macaroon":"sm","store-discharges":["sd"]}],
                "device":{"brand":"generic","model":"generic-classic",
                "serial":"fixture-serial","key-id":"fixture-key","session-macaroon":"fixture-macaroon"}},
        "refresh-privacy-key":"fixture-privacy","seeded":true,"snaps":{"core":{"active":true}}},
        "changes":{},"tasks":{}})";

    const std::optional<QByteArray> result = SnapdState::withoutDeviceIdentity(state);
    QVERIFY(result.has_value());
    const QJsonObject root = QJsonDocument::fromJson(*result).object();
    const QJsonObject data = root.value("data").toObject();
    QCOMPARE(data.value("auth").toObject().value("device").toObject(),
             QJsonObject({{"brand", "generic"}, {"model", "generic-classic"}}));
    QCOMPARE(data.value("auth").toObject().value("last-id").toInt(-1), 1);
    QVERIFY(!data.value("auth").toObject().contains("users"));
    QVERIFY(!data.value("auth").toObject().contains("macaroon-key"));
    QVERIFY(!data.contains("refresh-privacy-key"));
    QVERIFY(data.value("seeded").toBool());
    QVERIFY(data.value("snaps").toObject().contains("core"));
    QVERIFY(root.contains("changes"));
    QVERIFY(root.contains("tasks"));
}

void TestSnapdState::removesUsersWithoutDevice()
{
    const std::optional<QByteArray> result = SnapdState::withoutDeviceIdentity(
        R"({"data":{"auth":{"last-id":1,"macaroon-key":"Zml4dHVyZQ==","users":[{"id":1,"store-macaroon":"sm"}]}}})");
    QVERIFY(result.has_value());
    const QJsonObject data = QJsonDocument::fromJson(*result).object().value("data").toObject();
    QCOMPARE(data.value("auth").toObject(), QJsonObject({{"last-id", 1}}));
}

void TestSnapdState::keepsStateWithoutDevice()
{
    const std::optional<QByteArray> result
        = SnapdState::withoutDeviceIdentity(R"({"data":{"seeded":false},"changes":{}})");
    QVERIFY(result.has_value());
    const QJsonObject root = QJsonDocument::fromJson(*result).object();
    QCOMPARE(root.value("data").toObject(), QJsonObject({{"seeded", false}}));
}

void TestSnapdState::rejectsNonState_data()
{
    QTest::addColumn<QByteArray>("text");
    QTest::newRow("empty") << QByteArray();
    QTest::newRow("truncated") << QByteArray(R"({"data":{"auth":)");
    QTest::newRow("array") << QByteArray("[]");
    QTest::newRow("no data object") << QByteArray(R"({"data":[]})");
}

void TestSnapdState::rejectsNonState()
{
    QFETCH(QByteArray, text);
    QVERIFY(!SnapdState::withoutDeviceIdentity(text).has_value());
}

QTEST_GUILESS_MAIN(TestSnapdState)
#include "test_snapdstate.moc"
