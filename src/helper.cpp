/**********************************************************************
 *  helper.cpp
 **********************************************************************
 * Copyright (C) 2026 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *          OpenAI Codex
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package. If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QProcess>

#include <cstdio>
#include <sys/stat.h>

namespace
{
// Exit codes 126/127 are reserved: pkexec maps them to "authentication failed"
// and "authorization could not be obtained" respectively, and Cmd::helperProc()
// on the caller side treats either as a denied/cancelled elevation. pkexec itself
// cannot disambiguate this from an allow-listed command that legitimately exits
// with one of these codes — it forwards the child's real exit status verbatim —
// so this helper must never let either value escape, whether from its own
// internal errors or from a wrapped command's real exit code.
constexpr int HELPER_EXIT_INTERNAL_ERROR = 125; // this helper's own error (unknown/missing/unspawnable command)
constexpr int HELPER_EXIT_CHILD_REMAPPED = 124; // an allowed command legitimately exited 126 or 127

struct ProcessResult
{
    bool started = false;
    int exitCode = 1;
    QProcess::ExitStatus exitStatus = QProcess::NormalExit;
};

void writeAndFlush(FILE *stream, const QByteArray &data)
{
    if (!data.isEmpty()) {
        std::fwrite(data.constData(), 1, static_cast<size_t>(data.size()), stream);
        std::fflush(stream);
    }
}

void printError(const QString &message)
{
    writeAndFlush(stderr, message.toUtf8() + '\n');
}

[[nodiscard]] const QHash<QString, QStringList> &allowedCommands()
{
    static const QHash<QString, QStringList> commands {
        {"adduser", {"/usr/sbin/adduser"}},
        {"apt-get", {"/usr/bin/apt-get"}},
        {"bash", {"/usr/bin/bash", "/bin/bash"}},
        {"cat", {"/usr/bin/cat", "/bin/cat"}},
        {"chmod", {"/usr/bin/chmod", "/bin/chmod"}},
        {"chown", {"/usr/bin/chown", "/bin/chown"}},
        {"copy-initrd-programs",
         {"/usr/share/mx-snapshot/scripts/copy-initrd-programs",
          "/usr/share/iso-snapshot-cli/scripts/copy-initrd-programs"}},
        {"cp", {"/usr/bin/cp", "/bin/cp"}},
        {"deluser", {"/usr/sbin/deluser"}},
        {"du", {"/usr/bin/du", "/bin/du"}},
        {"install", {"/usr/bin/install", "/bin/install"}},
        {"installed-to-live", {"/usr/sbin/installed-to-live", "/usr/bin/installed-to-live"}},
        {"installed-to-live-arch",
         {"/usr/share/mx-snapshot/scripts/installed-to-live-arch",
          "/usr/share/iso-snapshot-cli/scripts/installed-to-live-arch"}},
        {"ln", {"/usr/bin/ln", "/bin/ln"}},
        {"localize-repo", {"/usr/bin/localize-repo"}},
        {"mkdir", {"/usr/bin/mkdir", "/bin/mkdir"}},
        {"mkinitcpio", {"/usr/bin/mkinitcpio"}},
        {"mksquashfs", {"/usr/bin/mksquashfs"}},
        {"modprobe", {"/usr/sbin/modprobe", "/sbin/modprobe", "/usr/bin/modprobe"}},
        {"mount", {"/usr/bin/mount", "/bin/mount"}},
        {"mountpoint", {"/usr/bin/mountpoint", "/bin/mountpoint"}},
        {"pacman", {"/usr/bin/pacman"}},
        {"readlink", {"/usr/bin/readlink", "/bin/readlink"}},
        {"rm", {"/usr/bin/rm", "/bin/rm"}},
        {"runuser", {"/usr/sbin/runuser", "/sbin/runuser", "/usr/bin/runuser"}},
        {"sh", {"/usr/bin/sh", "/bin/sh"}},
        {"stat", {"/usr/bin/stat", "/bin/stat"}},
        {"touch", {"/usr/bin/touch", "/bin/touch"}},
        {"true", {"/usr/bin/true", "/bin/true"}},
        {"umount", {"/usr/bin/umount", "/bin/umount"}},
        {"unbuffer", {"/usr/bin/unbuffer"}},
        {"userdel", {"/usr/sbin/userdel"}},
    };
    return commands;
}

[[nodiscard]] QString resolveBinary(const QStringList &candidates)
{
    for (const QString &candidate : candidates) {
        const QFileInfo info(candidate);
        if (info.exists() && info.isExecutable()) {
            return candidate;
        }
    }
    return {};
}

[[nodiscard]] ProcessResult runProcess(const QString &program, const QStringList &args, const QByteArray &input = {})
{
    ProcessResult result;

    QProcess process;
    QObject::connect(&process, &QProcess::readyReadStandardOutput, &process, [&process] {
        writeAndFlush(stdout, process.readAllStandardOutput());
    });
    QObject::connect(&process, &QProcess::readyReadStandardError, &process, [&process] {
        writeAndFlush(stderr, process.readAllStandardError());
    });

    process.start(program, args, QIODevice::ReadWrite);
    if (!process.waitForStarted()) {
        writeAndFlush(stderr, QString("Failed to start %1\n").arg(program).toUtf8());
        result.exitCode = HELPER_EXIT_INTERNAL_ERROR;
        return result;
    }

    result.started = true;
    if (!input.isEmpty()) {
        process.write(input);
    }
    process.closeWriteChannel();

    process.waitForFinished(-1);
    result.exitStatus = process.exitStatus();
    result.exitCode = process.exitCode();
    return result;
}

[[nodiscard]] int relayResult(const ProcessResult &result)
{
    if (!result.started) {
        return result.exitCode;
    }
    if (result.exitStatus != QProcess::NormalExit) {
        return 1;
    }
    if (result.exitCode == 126 || result.exitCode == 127) {
        printError(QString("Note: command exited %1, remapped to %2 to avoid the reserved "
                           "pkexec authentication exit codes")
                       .arg(result.exitCode)
                       .arg(HELPER_EXIT_CHILD_REMAPPED));
        return HELPER_EXIT_CHILD_REMAPPED;
    }
    return result.exitCode;
}

[[nodiscard]] QByteArray readHelperInput()
{
    QFile input;
    if (!input.open(stdin, QIODevice::ReadOnly)) {
        return {};
    }
    return input.readAll();
}

[[nodiscard]] int runAllowedCommand(const QString &command, const QStringList &commandArgs, const QByteArray &input = {})
{
    const auto commandIt = allowedCommands().constFind(command);
    if (commandIt == allowedCommands().constEnd()) {
        printError(QString("Command is not allowed: %1").arg(command));
        return HELPER_EXIT_INTERNAL_ERROR;
    }

    const QString resolvedCommand = resolveBinary(commandIt.value());
    if (resolvedCommand.isEmpty()) {
        printError(QString("Command is not available: %1").arg(command));
        return HELPER_EXIT_INTERNAL_ERROR;
    }

    return relayResult(runProcess(resolvedCommand, commandArgs, input));
}

// Resolve a live-files template path, preferring the /usr/local copy when it
// already exists (matches the paths used by the live-files package).
[[nodiscard]] QString resolveLiveFile(const QString &relativeName)
{
    const QString localPath = "/usr/local/share/live-files/files/etc/" + relativeName;
    if (QFile::exists(localPath)) {
        return localPath;
    }
    return "/usr/share/live-files/files/etc/" + relativeName;
}

// The values are written as file content only, never evaluated. Strip quotes
// and newlines so a crafted value cannot add extra lines or break the quoting
// of these shell-sourced template files.
[[nodiscard]] QString sanitizeTemplateValue(QString value)
{
    value.remove('"');
    value.replace('\n', ' ');
    value.replace('\r', ' ');
    return value;
}

[[nodiscard]] int writeLiveFile(const QString &relativeName, const QString &content)
{
    QFile file(resolveLiveFile(relativeName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        printError(QString("Could not write %1").arg(file.fileName()));
        return 1;
    }
    const QByteArray data = content.toUtf8();
    if (file.write(data) != data.size()) {
        printError(QString("Could not write %1").arg(file.fileName()));
        return 1;
    }
    return 0;
}

// Write the live-files mx-version template. These files are root-owned, so the
// GUI (which runs unprivileged) cannot write them directly.
[[nodiscard]] int writeVersionFile(const QStringList &args)
{
    if (args.size() != 3) {
        printError(QStringLiteral("write_version_file requires: <full_distro> <codename> <release_date>"));
        return 1;
    }
    const QString fullDistro = sanitizeTemplateValue(args.at(0));
    const QString codename = sanitizeTemplateValue(args.at(1));
    const QString releaseDate = sanitizeTemplateValue(args.at(2));
    return writeLiveFile(QStringLiteral("mx-version"),
                         QString("%1 %2 %3\n").arg(fullDistro, codename, releaseDate));
}

[[nodiscard]] int writeLsbRelease(const QStringList &args)
{
    if (args.size() != 3) {
        printError(QStringLiteral("write_lsb_release requires: <project> <version> <codename>"));
        return 1;
    }
    const QString project = sanitizeTemplateValue(args.at(0));
    const QString version = sanitizeTemplateValue(args.at(1));
    const QString codename = sanitizeTemplateValue(args.at(2));
    const QString content = QString("PRETTY_NAME=\"%1 %2 %3\"\n"
                                    "DISTRIB_ID=\"%1\"\n"
                                    "DISTRIB_RELEASE=%2\n"
                                    "DISTRIB_CODENAME=\"%3\"\n"
                                    "DISTRIB_DESCRIPTION=\"%1 %2 %3\"\n")
                                .arg(project, version, codename);
    return writeLiveFile(QStringLiteral("lsb-release"), content);
}
} // namespace

int main(int argc, char *argv[])
{
    ::umask(0022);

    QCoreApplication app(argc, argv);
    QStringList arguments = app.arguments().mid(1);
    if (arguments.isEmpty()) {
        printError(QStringLiteral("Missing helper action"));
        return 1;
    }

    const QString action = arguments.takeFirst();
    if (action == QLatin1String("write_version_file")) {
        return writeVersionFile(arguments);
    }
    if (action == QLatin1String("write_lsb_release")) {
        return writeLsbRelease(arguments);
    }
    if (action != QLatin1String("exec")) {
        printError(QString("Unsupported helper action: %1").arg(action));
        return 1;
    }
    if (arguments.isEmpty()) {
        printError(QStringLiteral("exec requires a command name"));
        return 1;
    }

    return runAllowedCommand(arguments.constFirst(), arguments.mid(1), readHelperInput());
}
