#include "unitsecrets.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

namespace UnitSecrets
{
namespace
{

// systemd joins a line ending in a backslash with the next one. Comment lines
// are ignored everywhere, so they neither start nor end a continuation.
QStringList logicalLines(const QString &text)
{
    QStringList lines;
    QString current;
    for (const QString &raw : text.split('\n')) {
        const QString trimmed = raw.trimmed();
        if (trimmed.startsWith('#') || trimmed.startsWith(';')) {
            continue;
        }
        if (trimmed.endsWith('\\')) {
            current += trimmed.chopped(1) + ' ';
            continue;
        }
        lines << current + trimmed;
        current.clear();
    }
    if (!current.isEmpty()) {
        lines << current;
    }
    return lines;
}

// Split an Environment= value into its assignments the way systemd does:
// whitespace separates words, and single or double quotes group one word.
QStringList environmentWords(const QString &value)
{
    QStringList words;
    QString word;
    bool inWord = false;
    QChar quote;
    for (qsizetype i = 0; i < value.size(); ++i) {
        const QChar c = value.at(i);
        if (!quote.isNull()) {
            if (c == '\\' && quote == '"' && i + 1 < value.size()) {
                word += value.at(++i);
            } else if (c == quote) {
                quote = QChar();
            } else {
                word += c;
            }
        } else if (c == '"' || c == '\'') {
            quote = c;
            inWord = true;
        } else if (c.isSpace()) {
            if (inWord) {
                words << word;
                word.clear();
                inWord = false;
            }
        } else {
            if (c == '\\' && i + 1 < value.size()) {
                word += value.at(++i);
            } else {
                word += c;
            }
            inWord = true;
        }
    }
    if (inWord) {
        words << word;
    }
    return words;
}

} // namespace

QStringList credentialNames(const QString &unitText)
{
    static const QRegularExpression environmentLine(QStringLiteral(R"(^Environment\s*=(.*)$)"));
    static const QRegularExpression assignment(QStringLiteral(R"(^([A-Za-z_][A-Za-z0-9_]*)=(.+)$)"));
    static const QRegularExpression secretName(
        QStringLiteral(R"(^(KEY|.*(TOKEN|SECRET|PASSWORD|PASSWD|_PASS|API_?KEY|AUTH_?KEY|PRIVATE_?KEY|CREDENTIAL)S?|.*_KEY)$)"),
        QRegularExpression::CaseInsensitiveOption);

    QStringList names;
    for (const QString &line : logicalLines(unitText)) {
        const QRegularExpressionMatch lineMatch = environmentLine.match(line);
        if (!lineMatch.hasMatch()) {
            continue;
        }
        for (const QString &word : environmentWords(lineMatch.captured(1))) {
            const QRegularExpressionMatch match = assignment.match(word);
            if (!match.hasMatch()) {
                continue;
            }
            const QString name = match.captured(1);
            if (secretName.match(name).hasMatch() && !names.contains(name)) {
                names << name;
            }
        }
    }
    return names;
}

ScanResult findCredentials(const QString &unitDir)
{
    ScanResult result;
    const QFileInfo rootInfo(unitDir);
    if (!rootInfo.exists()) {
        return result;
    }
    // A directory this (possibly unelevated) process can't list hides its
    // files, which are copied all the same: report it as not checked.
    const auto listable = [](const QFileInfo &info) { return info.isReadable() && info.isExecutable(); };
    if (!listable(rootInfo)) {
        result.unreadable << QDir::cleanPath(unitDir) + '/';
        return result;
    }

    const auto scanFile = [&result](const QString &path, const QString &relative) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            result.unreadable << relative;
            return;
        }
        if (file.size() > 1024 * 1024) {
            return;
        }
        const QStringList names = credentialNames(QString::fromUtf8(file.readAll()));
        for (const QString &name : names) {
            result.findings << relative + ": " + name;
        }
    };

    // Unit files sit at the top level; drop-ins in <unit>.d/*.conf. Symlinks
    // are enable/alias links to units scanned elsewhere (or to /usr/lib).
    const QDir root(unitDir);
    const QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden;
    for (const QFileInfo &entry : root.entryInfoList(filters)) {
        if (entry.isFile()) {
            scanFile(entry.filePath(), entry.fileName());
            continue;
        }
        if (!entry.fileName().endsWith(QLatin1String(".d"))) {
            continue;
        }
        if (!listable(entry)) {
            result.unreadable << entry.fileName() + '/';
            continue;
        }
        const QDir dropInDir(entry.filePath());
        const QFileInfoList dropIns
            = dropInDir.entryInfoList({QStringLiteral("*.conf")}, QDir::Files | QDir::NoSymLinks | QDir::Hidden);
        for (const QFileInfo &dropIn : dropIns) {
            scanFile(dropIn.filePath(), entry.fileName() + '/' + dropIn.fileName());
        }
    }
    result.findings.sort();
    result.unreadable.sort();
    return result;
}

} // namespace UnitSecrets
