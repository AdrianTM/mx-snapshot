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
    // Determine CLI mode using same logic as in main.cpp
#ifdef CLI_BUILD
    const bool useCliMode = true;
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
    const bool useCliMode = forceCliMode || noWindowSystem || headlessQpa;
#endif

    if (useCliMode) {
        if (QFile::exists("/usr/bin/sudo")) return QStringLiteral("/usr/bin/sudo");
        if (QFile::exists("/usr/bin/pkexec")) return QStringLiteral("/usr/bin/pkexec");
        if (QFile::exists("/usr/bin/gksu")) return QStringLiteral("/usr/bin/gksu");
        return {};
    }
    if (QFile::exists("/usr/bin/pkexec")) return QStringLiteral("/usr/bin/pkexec");
    if (QFile::exists("/usr/bin/gksu")) return QStringLiteral("/usr/bin/gksu");
    if (QFile::exists("/usr/bin/sudo")) return QStringLiteral("/usr/bin/sudo");
    return {};
}

QString Cmd::getOut(const QString &cmd, bool quiet, bool asRoot)
{
    out_buffer.clear();
    run(cmd, quiet, asRoot);
    return out_buffer.trimmed();
}

QString Cmd::getOutAsRoot(const QString &cmd, bool quiet)
{
    return getOut(cmd, quiet, true);
}

bool Cmd::run(const QString &cmd, bool quiet, bool asRoot)
{
    out_buffer.clear();
    if (state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << program() << arguments();
        return false;
    }
    if (!quiet) {
        qDebug().noquote() << cmd;
    }
    QEventLoop loop;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    if (asRoot && getuid() != 0) {
        start(elevate, {helper, cmd});
    } else {
        start("/bin/bash", {"-c", cmd});
    }
    loop.exec();
    emit done();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

bool Cmd::runAsRoot(const QString &cmd, bool quiet)
{
    return run(cmd, quiet, true);
}

QString Cmd::readAllOutput()
{
    return out_buffer.trimmed();
}
