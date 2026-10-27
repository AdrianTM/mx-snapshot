#pragma once

#include <QByteArray>
#include <optional>

namespace SnapdState
{

// snapd's /var/lib/snapd/state.json records this machine's registration with
// the snap store: auth.device holds the serial, the id of the device's private
// key and the store session, auth.users the accounts logged in with
// `snap login` (with their store credentials), auth.macaroon-key signs their
// local sessions, and refresh-privacy-key salts refresh requests. Return the
// state without them (brand and model stay, so snapd still knows what kind of
// device it is) and snapd registers as a new device and makes a new key on
// first boot. std::nullopt when the text isn't a snapd state.
[[nodiscard]] std::optional<QByteArray> withoutDeviceIdentity(const QByteArray &stateJson);

} // namespace SnapdState
