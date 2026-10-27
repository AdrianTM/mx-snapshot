#pragma once

#include <QString>
#include <QStringList>

namespace UnitSecrets
{

struct ScanResult {
    QStringList findings;   // "<relative path>: <VARIABLE>", never the value
    QStringList unreadable; // relative paths that could not be checked
};

// Scan systemd units and drop-ins under unitDir (normally /etc/systemd/system,
// where admin-created units live) for Environment= assignments whose variable
// name looks like a credential (TOKEN, PASSWORD, API_KEY, ...). Files that
// can't be read (e.g. root-only units seen from the unelevated GUI) are
// reported separately, since they are still copied into the snapshot.
[[nodiscard]] ScanResult findCredentials(const QString &unitDir);

// The same check applied to the text of a single unit file.
[[nodiscard]] QStringList credentialNames(const QString &unitText);

} // namespace UnitSecrets
