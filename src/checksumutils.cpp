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
    // set -e: a failed cp/pushd/checksum must fail the whole script instead of
    // being masked by the trailing temp-dir removal succeeding.
    return "set -e; TD='" + tempDir + "'; KEEP=\"$TD/.keep\"; [ -d \"$TD\" ] || mkdir \"$TD\"; "
           "FN=\"$1\"; CF=\"$2/$FN." + suffix + "\"; cp -- \"$FN\" \"$TD/$FN\"; "
           "pushd \"$TD\" >/dev/null; " + suffix + "sum -- \"$FN\" > \"$FN." + suffix + "\"; "
           "cp -- \"$FN." + suffix + "\" \"$CF\"; popd >/dev/null; [ -e \"$KEEP\" ] || rm -rf \"$TD\"";
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
