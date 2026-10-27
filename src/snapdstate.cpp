#include "snapdstate.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace SnapdState
{

std::optional<QByteArray> withoutDeviceIdentity(const QByteArray &stateJson)
{
    QJsonParseError error {};
    QJsonDocument document = QJsonDocument::fromJson(stateJson, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return std::nullopt;
    }
    QJsonObject root = document.object();
    if (!root.value(QStringLiteral("data")).isObject()) {
        return std::nullopt;
    }
    QJsonObject data = root.value(QStringLiteral("data")).toObject();
    data.remove(QStringLiteral("refresh-privacy-key"));

    if (data.value(QStringLiteral("auth")).isObject()) {
        QJsonObject auth = data.value(QStringLiteral("auth")).toObject();
        // Store logins (snap login) and the key snapd signs local macaroons
        // with; snapd makes a new key when someone logs in again.
        auth.remove(QStringLiteral("users"));
        auth.remove(QStringLiteral("macaroon-key"));
        if (auth.value(QStringLiteral("device")).isObject()) {
            QJsonObject device = auth.value(QStringLiteral("device")).toObject();
            for (const QString &key :
                 {QStringLiteral("serial"), QStringLiteral("key-id"), QStringLiteral("session-macaroon")}) {
                device.remove(key);
            }
            auth.insert(QStringLiteral("device"), device);
        }
        data.insert(QStringLiteral("auth"), auth);
    }
    root.insert(QStringLiteral("data"), data);
    document.setObject(root);
    return document.toJson(QJsonDocument::Compact);
}

} // namespace SnapdState
