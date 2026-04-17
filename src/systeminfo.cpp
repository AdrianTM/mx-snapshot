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
    if (QProcess::execute("mountpoint", {"-q", "/run/archiso/bootmnt"}) == 0) {
        return true;
    }
    return QProcess::execute("mountpoint", {"-q", "/live/aufs"}) == 0;
}

QStringList SystemInfo::listUsers()
{
    QStringList users = Cmd().getOut("lslogins --noheadings -u -o user", Cmd::QuietMode::Yes)
                            .split('\n', Qt::SkipEmptyParts);
    users.removeAll(QStringLiteral("root"));
    return users;
}

QString SystemInfo::readKernelOpts()
{
    const QString script = QString("/usr/share/%1/scripts/snapshot-bootparameter.sh")
                               .arg(QCoreApplication::applicationName());
    return Cmd().getOut(script).replace('\n', ' ');
}
