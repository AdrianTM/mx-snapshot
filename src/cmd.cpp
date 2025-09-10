#include "cmd.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>

#include <unistd.h>

Cmd::Cmd(QObject *parent)
    : QProcess(parent),
      helper {"/usr/lib/" + QCoreApplication::applicationName() + "/helper"}
{
    const QString tool = Cmd::elevationTool();
    elevate = tool.isEmpty() ? QStringLiteral("/usr/bin/sudo") : tool;
    connect(this, &Cmd::readyReadStandardOutput, [this] { emit outputAvailable(readAllStandardOutput()); });
    connect(this, &Cmd::readyReadStandardError, [this] { emit errorAvailable(readAllStandardError()); });
    connect(this, &Cmd::outputAvailable, [this](const QString &out) { out_buffer += out; });
    connect(this, &Cmd::errorAvailable, [this](const QString &out) { out_buffer += out; });
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
    const QStringList args = QCoreApplication::arguments();
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
    out_buffer.clear();
    run(cmd, quiet, elevation);
    return out_buffer.trimmed();
}

QString Cmd::getOutAsRoot(const QString &cmd, QuietMode quiet)
{
    return getOut(cmd, quiet, Elevation::Yes);
}

bool Cmd::run(const QString &cmd, QuietMode quiet, Elevation elevation)
{
    out_buffer.clear();
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (quiet == QuietMode::No) {
        qDebug().noquote() << cmd;
    }
    QEventLoop loop;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    if (elevation == Elevation::Yes && getuid() != 0) {
        start(elevate, {helper, cmd});
    } else {
        start("/bin/bash", {"-c", cmd});
    }
    loop.exec();
    emit done();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

bool Cmd::runAsRoot(const QString &cmd, QuietMode quiet)
{
    return run(cmd, quiet, Elevation::Yes);
}

QString Cmd::readAllOutput()
{
    return out_buffer.trimmed();
}
