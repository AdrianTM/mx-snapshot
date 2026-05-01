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
        {"apt-get", {"/usr/bin/apt-get"}},
        {"bash", {"/usr/bin/bash", "/bin/bash"}},
        {"chmod", {"/usr/bin/chmod", "/bin/chmod"}},
        {"chown", {"/usr/bin/chown", "/bin/chown"}},
        {"cp", {"/usr/bin/cp", "/bin/cp"}},
        {"cat", {"/usr/bin/cat", "/bin/cat"}},
        {"copy-initrd-programs",
         {"/usr/share/mx-snapshot/scripts/copy-initrd-programs",
          "/usr/share/iso-snapshot-cli/scripts/copy-initrd-programs"}},
        {"du", {"/usr/bin/du", "/bin/du"}},
        {"adduser", {"/usr/sbin/adduser"}},
        {"deluser", {"/usr/sbin/deluser"}},
        {"install", {"/usr/bin/install", "/bin/install"}},
        {"installed-to-live", {"/usr/sbin/installed-to-live", "/usr/bin/installed-to-live"}},
        {"ln", {"/usr/bin/ln", "/bin/ln"}},
        {"localize-repo", {"/usr/bin/localize-repo"}},
        {"mkdir", {"/usr/bin/mkdir", "/bin/mkdir"}},
        {"mount", {"/usr/bin/mount", "/bin/mount"}},
        {"mountpoint", {"/usr/bin/mountpoint", "/bin/mountpoint"}},
        {"mkinitcpio", {"/usr/bin/mkinitcpio"}},
        {"mksquashfs", {"/usr/bin/mksquashfs"}},
        {"modprobe", {"/usr/sbin/modprobe", "/sbin/modprobe", "/usr/bin/modprobe"}},
        {"pacman", {"/usr/bin/pacman"}},
        {"readlink", {"/usr/bin/readlink", "/bin/readlink"}},
        {"rm", {"/usr/bin/rm", "/bin/rm"}},
        {"runuser", {"/usr/sbin/runuser", "/sbin/runuser", "/usr/bin/runuser"}},
        {"sh", {"/usr/bin/sh", "/bin/sh"}},
        {"stat", {"/usr/bin/stat", "/bin/stat"}},
        {"stdbuf", {"/usr/bin/stdbuf"}},
        {"true", {"/usr/bin/true", "/bin/true"}},
        {"umount", {"/usr/bin/umount", "/bin/umount"}},
        {"touch", {"/usr/bin/touch", "/bin/touch"}},
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
        result.exitCode = 127;
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
    return result.exitStatus == QProcess::NormalExit ? result.exitCode : 1;
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
        return 127;
    }

    const QString resolvedCommand = resolveBinary(commandIt.value());
    if (resolvedCommand.isEmpty()) {
        printError(QString("Command is not available: %1").arg(command));
        return 127;
    }

    return relayResult(runProcess(resolvedCommand, commandArgs, input));
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
