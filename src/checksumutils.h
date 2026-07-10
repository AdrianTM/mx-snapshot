#pragma once

#include <QString>

namespace ChecksumUtils
{

// Both commands are bash scripts that expect the file name as $1 and the
// destination folder as $2, passed as positional parameters so the shell
// never parses them (no injection via a crafted file name or output folder).
// The suffix ("md5"/"sha512") and tempDir are trusted values supplied by the
// caller and are concatenated in. tempDir must be a private, per-run
// directory that already exists; its creation and removal are the caller's
// responsibility (see Work::makeChecksum's QTemporaryDir).
[[nodiscard]] QString directCommand(const QString &suffix);
[[nodiscard]] QString preemptCommand(const QString &suffix, const QString &tempDir);

[[nodiscard]] QString checksumFilePath(const QString &folder, const QString &fileName, const QString &suffix);
// A checksum run only counts as successful when the expected output file
// exists and is non-empty; a failed command can still leave nothing or an
// empty redirect target behind.
[[nodiscard]] bool verifyChecksumFile(const QString &folder, const QString &fileName, const QString &suffix);

} // namespace ChecksumUtils
