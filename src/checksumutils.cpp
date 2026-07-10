#include "checksumutils.h"

#include <QFileInfo>

namespace ChecksumUtils
{

QString directCommand(const QString &suffix)
{
    return suffix + "sum -- \"$1\" > \"$2/$1." + suffix + "\"";
}

QString preemptCommand(const QString &suffix, const QString &tempDir)
{
    // tempDir must be a private (0700), per-run directory created and removed
    // by the caller — the script only writes inside it and never creates or
    // deletes the directory itself, so it can't be lured into an
    // attacker-prepared path. set -e: a failed cp/cd/checksum must fail the
    // whole script instead of being masked by a later command succeeding.
    return "set -e; TD='" + tempDir + "'; "
           "FN=\"$1\"; CF=\"$2/$FN." + suffix + "\"; cp -- \"$FN\" \"$TD/$FN\"; "
           "cd \"$TD\"; " + suffix + "sum -- \"$FN\" > \"$FN." + suffix + "\"; "
           "cp -- \"$FN." + suffix + "\" \"$CF\"";
}

QString checksumFilePath(const QString &folder, const QString &fileName, const QString &suffix)
{
    return folder + "/" + fileName + "." + suffix;
}

bool verifyChecksumFile(const QString &folder, const QString &fileName, const QString &suffix)
{
    const QFileInfo info(checksumFilePath(folder, fileName, suffix));
    return info.isFile() && info.size() > 0;
}

} // namespace ChecksumUtils
