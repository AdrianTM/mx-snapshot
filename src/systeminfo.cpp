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
    return Cmd().getOut("lslogins --noheadings -u -o user |grep -vw root", Cmd::QuietMode::Yes).split('\n');
}

QString SystemInfo::readKernelOpts()
{
    return Cmd().getOut((QString("/usr/share/%1/scripts/snapshot-bootparameter.sh | tr '\n' ' '")
                             .arg(QCoreApplication::applicationName())));
}
