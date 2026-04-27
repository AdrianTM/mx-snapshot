#pragma once

#include <QString>
#include <QStringList>

namespace SquashfsUtils
{

struct Command {
    QString program;
    QStringList args;
};

struct Options {
    QString bindRootPath;
    QString outputPath;
    QString compression;
    uint cores = 1;
    uint throttle = 0;
    bool throttleSupported = false;
    bool percentageSupported = false;
    QString mksqOpt;
    QString excludesFileName;
    QString sessionExcludes;
    bool useUnbuffer = false;
};

[[nodiscard]] Command buildCommand(const Options &options);
[[nodiscard]] int parsePercentageLine(const QString &line, bool *ok = nullptr);

} // namespace SquashfsUtils
