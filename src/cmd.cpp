#include "cmd.h"

#include "elevationbroker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include <cstdlib>
#include <unistd.h>

Cmd::Cmd(QObject *parent)
    : QProcess(parent),
      elevationToolPath {Cmd::elevationTool()},
      helperPath {"/usr/lib/" + QCoreApplication::applicationName() + "/helper"}
{
    // Buffer everything; only emit the public signals when not suppressed.
    connect(this, &Cmd::readyReadStandardOutput, [this] {
        const QString out = QString::fromLocal8Bit(readAllStandardOutput());
        outBuffer += out;
        if (!suppressOutput) {
            emit outputAvailable(out);
        }
    });
    connect(this, &Cmd::readyReadStandardError, [this] {
        const QString out = QString::fromLocal8Bit(readAllStandardError());
        outBuffer += out;
        if (!suppressOutput) {
            emit errorAvailable(out);
        }
    });
}

QString Cmd::elevationTool()
{
    const bool cli = Cmd::isCliMode();
    if (cli) {
        if (QFile::exists("/usr/bin/sudo")) return QStringLiteral("/usr/bin/sudo");
        if (QFile::exists("/usr/bin/gksu")) return QStringLiteral("/usr/bin/gksu");
        if (QFile::exists("/usr/bin/pkexec")) return QStringLiteral("/usr/bin/pkexec");
        return {};
    }
    if (QFile::exists("/usr/bin/pkexec")) return QStringLiteral("/usr/bin/pkexec");
    if (QFile::exists("/usr/bin/gksu")) return QStringLiteral("/usr/bin/gksu");
    if (QFile::exists("/usr/bin/sudo")) return QStringLiteral("/usr/bin/sudo");
    return {};
}

QString Cmd::snapshotLibCommand(const QString &args)
{
    const QString snapshotLib = "/usr/lib/" + QCoreApplication::applicationName() + "/snapshot-lib";
    if (getuid() == 0) {
        return snapshotLib + " " + args;
    }
    // Prefer pkexec when available regardless of CLI/GUI mode: snapshot-lib calls
    // happen non-interactively (no TTY for a sudo password prompt), and the
    // accompanying polkit policy uses auth_admin_keep for this helper.
    if (QFile::exists("/usr/bin/pkexec")) {
        return QStringLiteral("/usr/bin/pkexec ") + snapshotLib + " " + args;
    }
    const QString elevate = Cmd::elevationTool();
    if (elevate.isEmpty()) {
        return snapshotLib + " " + args;
    }
    return elevate + " " + snapshotLib + " " + args;
}

bool Cmd::runSnapshotLib(const QString &args, QuietMode quiet)
{
    Cmd cmd;
    return cmd.run(snapshotLibCommand(args), quiet);
}

bool Cmd::isCliMode()
{
#ifdef CLI_BUILD
    return true;
#else
    if (QCoreApplication::instance() && !QCoreApplication::instance()->inherits("QApplication")) {
        return true;
    }
    const auto args = QCoreApplication::arguments();
    const bool forceCliMode = args.contains("--cli") || args.contains("-c") ||
                              args.contains("--help") || args.contains("-h") ||
                              QFileInfo(QCoreApplication::applicationFilePath()).baseName().endsWith("cli") ||
                              !qgetenv("MX_SNAPSHOT_CLI").isEmpty();
    const bool noWindowSystem = qgetenv("DISPLAY").isEmpty() && qgetenv("WAYLAND_DISPLAY").isEmpty();
    const QString qpa = QString::fromLocal8Bit(qgetenv("QT_QPA_PLATFORM"));
    const bool headlessQpa = (qpa == QLatin1String("offscreen") ||
                              qpa == QLatin1String("minimal") ||
                              qpa == QLatin1String("linuxfb"));
    return forceCliMode || noWindowSystem || headlessQpa;
#endif
}

QString Cmd::loggedInUserName()
{
    QString username = qEnvironmentVariable("SUDO_USER");
    if (username.isEmpty()) {
        username = qEnvironmentVariable("LOGNAME");
    }
    if (username.isEmpty()) {
        username = qEnvironmentVariable("USER");
    }
    if (username.isEmpty() || username == QLatin1String("root")) {
        username = Cmd().getOut("logname", Cmd::QuietMode::Yes).trimmed();
    }
    return username == QLatin1String("root") ? QString() : username;
}

QStringList Cmd::helperExecArgs(const QString &cmd, const QStringList &args)
{
    QStringList helperArgs {"exec", cmd};
    helperArgs += args;
    return helperArgs;
}

QString Cmd::getOut(const QString &cmd, QuietMode quiet)
{
    QString output;
    run(cmd, quiet);
    output = outBuffer;
    return output.trimmed();
}

QString Cmd::getOutAsRoot(const QString &cmd, const QStringList &args, QuietMode quiet)
{
    QString output;
    procAsRoot(cmd, args, &output, nullptr, quiet);
    return output;
}

QString Cmd::getOutAsRoot(const QString &cmd, QuietMode quiet)
{
    QString output;
    procAsRoot("bash", {"-c", cmd}, &output, nullptr, quiet);
    return output;
}

bool Cmd::helperProc(const QStringList &helperArgs, QString *output, const QByteArray *input, QuietMode quiet)
{
    if (getuid() != 0 && elevationToolPath.isEmpty()) {
        const QString message = tr("No elevation tool found (pkexec/gksu/sudo).");
        qWarning().noquote() << message;
        emit errorAvailable(message);
        emit done();
        return false;
    }

    // Unprivileged: go through the persistent broker so the user authenticates
    // once — on the first elevated command — instead of once per ~5-minute
    // auth_admin_keep window (a long mksquashfs always outlives the grant).
    if (getuid() != 0) {
        auto &broker = ElevationBroker::instance();
        switch (broker.ensureStarted(helperPath, elevationToolPath)) {
        case ElevationBroker::Launch::Denied:
            handleElevationError();
            emit done();
            return false;
        case ElevationBroker::Launch::Ready: {
            if (quiet == QuietMode::No) {
                qDebug() << "helper(serve)" << helperArgs;
            }
            outBuffer.clear();
            const auto sink = [this](const QByteArray &chunk, bool isError) {
                const QString text = QString::fromLocal8Bit(chunk);
                outBuffer += text;
                if (!suppressOutput) {
                    if (isError) {
                        emit errorAvailable(text);
                    } else {
                        emit outputAvailable(text);
                    }
                }
            };
            const int code = broker.execute(
                helperArgs, input != nullptr ? *input : QByteArray(),
                [&sink](const QByteArray &chunk) { sink(chunk, false); },
                [&sink](const QByteArray &chunk) { sink(chunk, true); });
            if (output != nullptr) {
                *output = outBuffer.trimmed();
            }
            emit done();
            return code == 0;
        }
        case ElevationBroker::Launch::Failed:
            // No broker (helper missing, exec failure, ...): fall back to the
            // one-shot per-call elevation below.
            qWarning() << "Elevation broker unavailable, using per-call elevation";
            break;
        }
    }

    const QString program = (getuid() == 0) ? helperPath : elevationToolPath;
    QStringList programArgs = helperArgs;
    if (getuid() != 0) {
        programArgs.prepend(helperPath);
    }

    const bool result = proc(program, programArgs, output, input, quiet, Elevation::No);
    if (exitCode() == EXIT_CODE_PERMISSION_DENIED || exitCode() == EXIT_CODE_COMMAND_NOT_FOUND) {
        handleElevationError();
    }
    return result;
}

bool Cmd::proc(const QString &cmd, const QStringList &args, QString *output, const QByteArray *input, QuietMode quiet,
               Elevation elevation)
{
    if (elevation == Elevation::Yes) {
        return helperProc(helperExecArgs(cmd, args), output, input, quiet);
    }

    outBuffer.clear();
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (quiet == QuietMode::No) {
        qDebug() << cmd << args;
    }

    QEventLoop loop;
    bool processFinished = false;
    // Track the connection so we can disconnect after this invocation; otherwise
    // a later QProcess::finished can call into a stale local event loop.
    const auto conn = connect(this, &QProcess::finished, &loop, [&loop, &processFinished](int, QProcess::ExitStatus) {
        processFinished = true;
        loop.quit();
    });
    start(cmd, args);
    if (!waitForStarted()) {
        const QString message = tr("Failed to start command: %1").arg(cmd);
        qWarning().noquote() << message << errorString();
        emit errorAvailable(message + ": " + errorString());
        if (output) {
            *output = outBuffer.trimmed();
        }
        emit done();
        disconnect(conn);
        return false;
    }
    if (input && !input->isEmpty()) {
        write(*input);
    }
    closeWriteChannel();
    if (!processFinished) {
        loop.exec();
    }
    disconnect(conn);

    if (output) {
        *output = outBuffer.trimmed();
    }
    emit done();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

bool Cmd::procAsRoot(const QString &cmd, const QStringList &args, QString *output, const QByteArray *input,
                     QuietMode quiet)
{
    return proc(cmd, args, output, input, quiet, Elevation::Yes);
}

bool Cmd::runHelperAction(const QString &action, const QStringList &args, QString *output, QuietMode quiet)
{
    QStringList helperArgs {action};
    helperArgs += args;
    return helperProc(helperArgs, output, nullptr, quiet);
}

bool Cmd::run(const QString &cmd, QuietMode quiet)
{
    return proc("/bin/bash", {"-c", cmd}, nullptr, nullptr, quiet);
}

bool Cmd::runAsRoot(const QString &cmd, QuietMode quiet)
{
    return procAsRoot("bash", {"-c", cmd}, nullptr, nullptr, quiet);
}

QString Cmd::readAllOutput()
{
    return outBuffer.trimmed();
}

void Cmd::setOutputSuppressed(bool suppressed)
{
    suppressOutput = suppressed;
}

bool Cmd::outputSuppressed() const
{
    return suppressOutput;
}

void Cmd::handleElevationError()
{
    // Do not abort here: record the denial and let the caller decide. A user who
    // cancels the authentication dialog should land back at the stage that asked
    // for elevation, not have the application exit under them.
    s_elevationDenied = true;
    const QString message = tr("Administrator access was not granted (authentication cancelled or denied).");
    qWarning().noquote() << message;
    emit errorAvailable(message + '\n');
}
