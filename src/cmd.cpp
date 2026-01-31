#include "cmd.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QTimer>

#include <unistd.h>

#include "messagehandler.h"

Cmd::Cmd(QObject *parent)
    : QProcess(parent),
      elevationToolPath {Cmd::elevationTool()},
      helperPath {"/usr/lib/" + QCoreApplication::applicationName() + "/helper"}
{
    connect(this, &Cmd::readyReadStandardOutput, [this] { emit outputAvailable(readAllStandardOutput()); });
    connect(this, &Cmd::readyReadStandardError, [this] { emit errorAvailable(readAllStandardError()); });
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

QString Cmd::getOut(const QString &cmd, QuietMode quiet, Elevation elevation)
{
    outBuffer.clear();
    run(cmd, quiet, elevation);
    return outBuffer.trimmed();
}

QString Cmd::getOutAsRoot(const QString &cmd, QuietMode quiet)
{
    return getOut(cmd, quiet, Elevation::Yes);
}

bool Cmd::run(const QString &cmd, QuietMode quiet, Elevation elevation)
{
    outBuffer.clear();
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (quiet == QuietMode::No) {
        qDebug().noquote() << cmd;
    }
    if (elevation == Elevation::Yes && getuid() != 0 && elevationToolPath.isEmpty()) {
        const QString message = tr("No elevation tool found (pkexec/gksu/sudo).");
        qWarning().noquote() << message;
        emit errorAvailable(message);
        emit done();
        return false;
    }
    QEventLoop loop;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    if (elevation == Elevation::Yes && getuid() != 0) {
        start(elevationToolPath, {helperPath, cmd});
    } else {
        start("/bin/bash", {"-c", cmd});
    }
    loop.exec();
    if (elevation == Elevation::Yes && getuid() != 0
        && (exitCode() == EXIT_CODE_PERMISSION_DENIED || exitCode() == EXIT_CODE_COMMAND_NOT_FOUND)) {
        handleElevationError();
    }
    emit done();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

bool Cmd::runAsRoot(const QString &cmd, QuietMode quiet)
{
    return run(cmd, quiet, Elevation::Yes);
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
