#include "squashfsutils.h"

#include <QProcess>
#include <QRegularExpression>

namespace
{
QStringList splitShellWords(const QString &text)
{
    return text.trimmed().isEmpty() ? QStringList() : QProcess::splitCommand(text);
}
} // namespace

namespace SquashfsUtils
{

Command buildCommand(const Options &options)
{
    QStringList squashfsArgs {options.bindRootPath, options.outputPath,
                              "-comp", options.compression,
                              "-processors", QString::number(options.cores)};
    if (options.throttleSupported) {
        squashfsArgs << "-throttle" << QString::number(options.throttle);
    }
    if (options.percentageSupported) {
        squashfsArgs << "-percentage";
    }
    squashfsArgs += splitShellWords(options.mksqOpt);
    squashfsArgs << "-wildcards" << "-ef" << options.excludesFileName;

    const QStringList sessionExcludes = splitShellWords(options.sessionExcludes);
    if (!sessionExcludes.isEmpty()) {
        squashfsArgs << "-e";
        squashfsArgs += sessionExcludes;
    }

    Command command;
    command.program = options.useUnbuffer ? QStringLiteral("unbuffer") : QStringLiteral("mksquashfs");
    if (options.useUnbuffer) {
        command.args << "mksquashfs";
    }
    command.args += squashfsArgs;
    return command;
}

bool helpListsOption(const QString &helpText, const QString &option)
{
    const QRegularExpression optionLine(
        QStringLiteral("(?:^|\\s)%1(?:\\s|$)").arg(QRegularExpression::escape(option)));
    return optionLine.match(helpText).hasMatch();
}

int parsePercentageLine(const QString &line, bool *ok)
{
    static const QRegularExpression ansiEscape(QStringLiteral("\\x1B\\[[0-?]*[ -/]*[@-~]"));
    static const QRegularExpression squashfsPercentageLine(QStringLiteral("^\\s*(100|\\d{1,2})\\s*%?\\s*$"));

    const QString cleanedLine = QString(line).remove(ansiEscape);
    const QRegularExpressionMatch match = squashfsPercentageLine.match(cleanedLine);
    if (!match.hasMatch()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }

    bool converted = false;
    const int percentage = match.captured(1).toInt(&converted);
    if (ok) {
        *ok = converted;
    }
    return converted ? percentage : 0;
}

} // namespace SquashfsUtils
