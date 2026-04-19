#include "systeminfo.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QSysInfo>

#include "cmd.h"

bool SystemInfo::is386()
{
    return QSysInfo::currentCpuArchitecture() == "i386";
}

bool SystemInfo::isLive()
{
    return QProcess::execute("mountpoint", {"-q", "/live/aufs"}) == 0;
}

QStringList SystemInfo::listUsers()
{
    const QStringList lines = Cmd().getOut("lslogins --noheadings -u -o user,HOMEDIR", Cmd::QuietMode::Yes)
                                  .split('\n', Qt::SkipEmptyParts);
    QStringList users;
    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        const int sep = trimmed.indexOf(' ');
        if (sep > 0 && trimmed.mid(sep + 1).trimmed().startsWith("/home/")) {
            users << trimmed.left(sep);
        }
    }
    return users;
}

QString SystemInfo::readKernelOpts()
{
    const QString script = QString("/usr/share/%1/scripts/snapshot-bootparameter.sh")
                               .arg(QCoreApplication::applicationName());
    return Cmd().getOut(script).replace('\n', ' ');
}
