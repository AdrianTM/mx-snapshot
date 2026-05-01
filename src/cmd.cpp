#include "cmd.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QStringList>
#include <QTimer>

#include <unistd.h>

#include "messagehandler.h"

Cmd::Cmd(QObject *parent)
    : QProcess(parent),
      elevationToolPath {Cmd::elevationTool()},
      helperPath {"/usr/lib/" + QCoreApplication::applicationName() + "/helper"}
{
    connect(this, &Cmd::readyReadStandardOutput,
            [this] { emit outputAvailable(QString::fromLocal8Bit(readAllStandardOutput())); });
    connect(this, &Cmd::readyReadStandardError,
            [this] { emit errorAvailable(QString::fromLocal8Bit(readAllStandardError())); });
    connect(this, &Cmd::outputAvailable, [this](const QString &out) { outBuffer += out; });
    connect(this, &Cmd::errorAvailable, [this](const QString &out) { outBuffer += out; });
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

bool Cmd::isCliMode()
{
#ifdef CLI_BUILD
    return true;
#else
    const auto args = QCoreApplication::arguments();
    const bool forceCliMode = args.contains("--cli") || args.contains("-c") ||
                              args.contains("--help") || args.contains("-h") ||
                              QCoreApplication::applicationFilePath().contains("cli") ||
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

bool Cmd::helperProc(const QStringList &helperArgs, QString *output, const QByteArray *input, QuietMode quiet)
{
    if (getuid() != 0 && elevationToolPath.isEmpty()) {
        const QString message = tr("No elevation tool found (pkexec/gksu/sudo).");
        qWarning().noquote() << message;
        emit errorAvailable(message);
        emit done();
        return false;
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
    connect(this, &QProcess::finished, &loop, [&loop, &processFinished](int, QProcess::ExitStatus) {
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
        return false;
    }
    if (input && !input->isEmpty()) {
        write(*input);
    }
    closeWriteChannel();
    if (!processFinished) {
        loop.exec();
    }

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

bool Cmd::run(const QString &cmd, QuietMode quiet)
{
    return proc("/bin/bash", {"-c", cmd}, nullptr, nullptr, quiet);
}

QString Cmd::readAllOutput()
{
    return outBuffer.trimmed();
}

void Cmd::handleElevationError()
{
    MessageHandler::showMessage(MessageHandler::Critical, tr("Administrator Access Required"),
                                tr("This operation requires administrator privileges. Please restart the "
                                   "application and enter your password when prompted."));
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
    exit(EXIT_FAILURE);
}
