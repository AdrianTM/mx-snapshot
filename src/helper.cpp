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
#include <QQueue>
#include <QSocketNotifier>

#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

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
// ------------------------------------------------------------------ serve
// Broker mode: one authenticated helper instance stays alive for the whole
// snapshot run and executes framed requests from its parent (the unprivileged
// frontend), so polkit's ~5-minute auth_admin_keep window cannot expire
// between privileged steps (mksquashfs alone outlasts it).
//
// Protocol (stdin = requests, stdout = frames, stderr = raw passthrough):
//   client -> broker:
//     "REQ <id> <argc> <inlen>\n" then argc x ("<len>\n" + bytes) helper args
//     (same vectors as one-shot mode, e.g. "exec" "mksquashfs" ...), then
//     inlen bytes of child stdin content.
//     "KILL\n" terminates the currently running child's process group
//     (out-of-band: it is parsed even while a child runs, so a cancel can
//     stop mksquashfs without waiting behind it in the queue).
//   broker -> client:
//     "READY\n" once after startup (i.e. after pkexec authentication).
//     "O <id> <len>\n" + bytes  child stdout chunk.
//     "R <id> <code>\n"         request finished (relayResult semantics).
// Requests are executed one at a time in FIFO order. The broker exits on
// stdin EOF (client gone), killing any still-running child first.
class BrokerServer : public QObject
{
public:
    explicit BrokerServer(QObject *parent = nullptr)
        : QObject(parent)
    {
        const int flags = ::fcntl(STDIN_FILENO, F_GETFL, 0);
        ::fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &BrokerServer::readInput);
        writeLine(QByteArrayLiteral("READY"));
    }

private:
    struct Request
    {
        quint64 id = 0;
        QStringList argv;
        QByteArray input;
    };
    enum class ParseState { Header, ArgLength, ArgBytes, InputBytes };

    // Caps guard against a corrupted stream, not against the client (which is
    // trusted to the same degree as in one-shot mode).
    static constexpr int MAX_ARGS = 4096;
    static constexpr qint64 MAX_ARG_LEN = 1LL << 20;   // 1 MiB per argument
    static constexpr qint64 MAX_INPUT_LEN = 1LL << 26; // 64 MiB of child stdin

    void readInput()
    {
        char chunk[65536];
        while (true) {
            const ssize_t n = ::read(STDIN_FILENO, chunk, sizeof(chunk));
            if (n > 0) {
                buffer.append(chunk, static_cast<qsizetype>(n));
                continue;
            }
            if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                break;
            }
            eofSeen = true; // EOF or read error: the client is gone.
            notifier->setEnabled(false);
            break;
        }
        parse();
        maybeQuit();
    }

    void parse()
    {
        while (true) {
            if (state == ParseState::Header) {
                const qsizetype nl = buffer.indexOf('\n');
                if (nl < 0) {
                    return;
                }
                const QByteArray line = buffer.left(nl);
                buffer.remove(0, nl + 1);
                const QList<QByteArray> parts = line.split(' ');
                if (parts.value(0) == "KILL") {
                    killChild();
                    continue;
                }
                if (parts.value(0) == "REQ" && parts.size() == 4) {
                    bool okId = false;
                    bool okArgc = false;
                    bool okIn = false;
                    pendingRequest = Request {};
                    pendingRequest.id = parts.at(1).toULongLong(&okId);
                    argsRemaining = parts.at(2).toInt(&okArgc);
                    inputRemaining = parts.at(3).toLongLong(&okIn);
                    if (!okId || !okArgc || !okIn || argsRemaining < 1 || argsRemaining > MAX_ARGS
                        || inputRemaining < 0 || inputRemaining > MAX_INPUT_LEN) {
                        protocolError("bad REQ header");
                    }
                    state = ParseState::ArgLength;
                    continue;
                }
                protocolError("unknown frame");
            }
            if (state == ParseState::ArgLength) {
                const qsizetype nl = buffer.indexOf('\n');
                if (nl < 0) {
                    return;
                }
                bool ok = false;
                argRemaining = buffer.left(nl).toLongLong(&ok);
                buffer.remove(0, nl + 1);
                if (!ok || argRemaining < 0 || argRemaining > MAX_ARG_LEN) {
                    protocolError("bad argument length");
                }
                state = ParseState::ArgBytes;
            }
            if (state == ParseState::ArgBytes) {
                if (buffer.size() < argRemaining) {
                    return;
                }
                pendingRequest.argv << QString::fromUtf8(buffer.left(argRemaining));
                buffer.remove(0, argRemaining);
                --argsRemaining;
                state = (argsRemaining == 0) ? ParseState::InputBytes : ParseState::ArgLength;
                if (state == ParseState::ArgLength) {
                    continue;
                }
            }
            if (state == ParseState::InputBytes) {
                if (buffer.size() < inputRemaining) {
                    return;
                }
                pendingRequest.input = buffer.left(inputRemaining);
                buffer.remove(0, inputRemaining);
                queue.enqueue(pendingRequest);
                state = ParseState::Header;
                startNext();
            }
        }
    }

    [[noreturn]] void protocolError(const char *what)
    {
        // Framing is lost; there is no way to resynchronize the stream.
        printError(QString("serve: protocol error (%1)").arg(what));
        std::exit(HELPER_EXIT_INTERNAL_ERROR);
    }

    void startNext()
    {
        if (child != nullptr || queue.isEmpty()) {
            return;
        }
        current = queue.dequeue();
        QStringList argv = current.argv;
        const QString action = argv.takeFirst();
        if (action == QLatin1String("write_version_file")) {
            finish(writeVersionFile(argv));
            return;
        }
        if (action == QLatin1String("write_lsb_release")) {
            finish(writeLsbRelease(argv));
            return;
        }
        if (action != QLatin1String("exec") || argv.isEmpty()) {
            printError(QString("serve: unsupported action: %1").arg(action));
            finish(HELPER_EXIT_INTERNAL_ERROR);
            return;
        }
        const QString name = argv.takeFirst();
        const auto commandIt = allowedCommands().constFind(name);
        if (commandIt == allowedCommands().constEnd()) {
            printError(QString("Command is not allowed: %1").arg(name));
            finish(HELPER_EXIT_INTERNAL_ERROR);
            return;
        }
        const QString resolvedCommand = resolveBinary(commandIt.value());
        if (resolvedCommand.isEmpty()) {
            printError(QString("Command is not available: %1").arg(name));
            finish(HELPER_EXIT_INTERNAL_ERROR);
            return;
        }

        child = new QProcess(this);
        // Own process group, so KILL can stop a whole pipeline (unbuffer/bash
        // wrappers included), mirroring what pkill did in one-shot mode.
        child->setChildProcessModifier([] { ::setpgid(0, 0); });
        connect(child, &QProcess::readyReadStandardOutput, this,
                [this] { writeOutputChunk(child->readAllStandardOutput()); });
        connect(child, &QProcess::readyReadStandardError, this,
                [this] { writeAndFlush(stderr, child->readAllStandardError()); });
        connect(child, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
            ProcessResult result;
            result.started = true;
            result.exitCode = exitCode;
            result.exitStatus = exitStatus;
            child->deleteLater();
            child = nullptr;
            finish(relayResult(result));
        });
        child->start(resolvedCommand, argv, QIODevice::ReadWrite);
        if (!child->waitForStarted()) {
            printError(QString("Failed to start %1").arg(resolvedCommand));
            child->deleteLater();
            child = nullptr;
            finish(HELPER_EXIT_INTERNAL_ERROR);
            return;
        }
        if (!current.input.isEmpty()) {
            child->write(current.input);
        }
        child->closeWriteChannel();
    }

    void finish(int code)
    {
        writeLine("R " + QByteArray::number(current.id) + ' ' + QByteArray::number(code));
        maybeQuit();
        startNext();
    }

    void killChild()
    {
        if (child != nullptr && child->processId() > 0) {
            ::kill(-static_cast<pid_t>(child->processId()), SIGTERM);
        }
    }

    void maybeQuit()
    {
        if (!eofSeen) {
            return;
        }
        queue.clear();
        if (child != nullptr) {
            killChild(); // its finished handler re-enters maybeQuit()
            return;
        }
        QCoreApplication::quit();
    }

    void writeOutputChunk(const QByteArray &data)
    {
        if (data.isEmpty()) {
            return;
        }
        const QByteArray header
            = "O " + QByteArray::number(current.id) + ' ' + QByteArray::number(data.size()) + '\n';
        std::fwrite(header.constData(), 1, static_cast<size_t>(header.size()), stdout);
        std::fwrite(data.constData(), 1, static_cast<size_t>(data.size()), stdout);
        std::fflush(stdout);
    }

    static void writeLine(const QByteArray &line)
    {
        writeAndFlush(stdout, line + '\n');
    }

    QSocketNotifier *notifier = nullptr;
    QProcess *child = nullptr;
    QByteArray buffer;
    QQueue<Request> queue;
    Request pendingRequest;
    Request current;
    ParseState state = ParseState::Header;
    int argsRemaining = 0;
    qint64 argRemaining = 0;
    qint64 inputRemaining = 0;
    bool eofSeen = false;
};
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
    if (action == QLatin1String("serve")) {
        // Writes to a closed pipe (client crashed) must not kill the broker
        // mid-child; fwrite just fails and EOF handling exits cleanly.
        ::signal(SIGPIPE, SIG_IGN);
        BrokerServer server;
        return QCoreApplication::exec();
    }
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
