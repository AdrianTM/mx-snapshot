/**********************************************************************
 *  bindrootmanager.cpp
 **********************************************************************
 * Copyright (C) 2025 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of MX Snapshot.
 *
 * MX Snapshot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MX Snapshot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MX Snapshot.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "bindrootmanager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTemporaryFile>
#include <QTextStream>
#include <QThread>

#include <algorithm>
#include <crypt.h>

namespace
{
QString base64Encode(const QByteArray &data)
{
    return QString::fromLatin1(data.toBase64());
}

QString randomHex32()
{
    QByteArray bytes;
    bytes.reserve(16);
    for (int i = 0; i < 4; ++i) {
        const quint32 value = QRandomGenerator::global()->generate();
        bytes.append(reinterpret_cast<const char *>(&value), static_cast<int>(sizeof(value)));
    }
    return QString::fromLatin1(bytes.toHex());
}

QString randomSalt(int length)
{
    static const char kSaltChars[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789./";
    QString salt;
    salt.reserve(length);
    for (int i = 0; i < length; ++i) {
        const int index = QRandomGenerator::global()->bounded(static_cast<int>(sizeof(kSaltChars) - 1));
        salt.append(kSaltChars[index]);
    }
    return salt;
}

QString sha512Crypt(const QString &password)
{
    const QString salt = "$6$" + randomSalt(16) + "$";
    const QByteArray pw = password.toUtf8();
    const QByteArray saltBytes = salt.toUtf8();
    const char *hash = ::crypt(pw.constData(), saltBytes.constData());
    if (!hash) {
        return {};
    }
    return QString::fromLatin1(hash);
}

QString quoted(const QString &value)
{
    QString escaped = value;
    escaped.replace('"', "\\\"");
    return "\"" + escaped + "\"";
}

QString appName()
{
    return QCoreApplication::applicationName();
}

QString stateFileName()
{
    return QStringLiteral("cleanup.json");
}

QString primaryStateDir()
{
    return QStringLiteral("/run/") + appName();
}

QString fallbackStateDir()
{
    return QStringLiteral("/tmp/") + appName();
}

} // namespace

BindRootManager::BindRootManager(Cmd &shell, const QString &bindRoot, const QString &workDir)
    : shell(shell),
      bindRoot(bindRoot),
      workDir(workDir)
{
}

bool BindRootManager::hasCleanupState()
{
    return QFileInfo::exists(primaryStateDir() + "/" + stateFileName())
        || QFileInfo::exists(fallbackStateDir() + "/" + stateFileName());
}

QString BindRootManager::stateDirPath()
{
    if (QFileInfo::exists(primaryStateDir() + "/" + stateFileName())) {
        return primaryStateDir();
    }
    if (QFileInfo::exists(fallbackStateDir() + "/" + stateFileName())) {
        return fallbackStateDir();
    }
    if (QFileInfo::exists(primaryStateDir())) {
        return primaryStateDir();
    }
    return fallbackStateDirPath();
}

QString BindRootManager::fallbackStateDirPath() const
{
    return fallbackStateDir();
}

QString BindRootManager::stateFilePath()
{
    return stateDirPath() + "/" + stateFileName();
}

bool BindRootManager::ensureStateDir()
{
    const QString primaryDir = primaryStateDir();
    if (shell.runAsRoot("install -d -m 0755 " + quoted(primaryDir), Cmd::QuietMode::Yes)) {
        return true;
    }
    const QString fallbackDir = fallbackStateDirPath();
    return shell.runAsRoot("install -d -m 0755 " + quoted(fallbackDir), Cmd::QuietMode::Yes);
}

bool BindRootManager::persistState()
{
    if (!ensureStateDir()) {
        qWarning() << "Failed to create cleanup state directory.";
        return false;
    }

    QJsonObject state;
    state["realRoot"] = realRoot;
    state["bindRoot"] = bindRoot;
    state["workDir"] = workDir;
    state["rmDirs"] = QJsonArray::fromStringList(rmDirs);
    state["rmFiles"] = QJsonArray::fromStringList(rmFiles);

    const QByteArray json = QJsonDocument(state).toJson(QJsonDocument::Compact);
    const QString encoded = base64Encode(json);
    const QString stateFile = stateFilePath();
    const QString cmd = "printf '%s' '" + encoded + "' | base64 -d > " + quoted(stateFile)
        + " && chmod 0644 " + quoted(stateFile);
    if (!shell.runAsRoot(cmd, Cmd::QuietMode::Yes)) {
        qWarning() << "Failed to persist cleanup state.";
        return false;
    }
    stateLoaded = true;
    return true;
}

bool BindRootManager::loadState()
{
    const QString stateFile = stateFilePath();
    QFile file(stateFile);
    QByteArray data;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        data = file.readAll();
    } else {
        const QString content = shell.getOutAsRoot("cat " + quoted(stateFile), Cmd::QuietMode::Yes);
        data = content.toUtf8();
    }
    if (data.isEmpty()) {
        return false;
    }
    QJsonParseError error {};
    const QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }
    const QJsonObject obj = doc.object();
    realRoot = obj.value("realRoot").toString("/");
    bindRoot = obj.value("bindRoot").toString(bindRoot);
    workDir = obj.value("workDir").toString(workDir);
    rmDirs.clear();
    rmFiles.clear();
    const QJsonArray dirsArray = obj.value("rmDirs").toArray();
    for (const QJsonValue &value : dirsArray) {
        if (value.isString()) {
            rmDirs.append(value.toString());
        }
    }
    const QJsonArray filesArray = obj.value("rmFiles").toArray();
    for (const QJsonValue &value : filesArray) {
        if (value.isString()) {
            rmFiles.append(value.toString());
        }
    }
    stateLoaded = true;
    return true;
}

void BindRootManager::addRmFile(const QString &path)
{
    if (!rmFiles.contains(path)) {
        rmFiles.append(path);
        persistState();
    }
}

void BindRootManager::addRmDir(const QString &path)
{
    if (!rmDirs.contains(path)) {
        rmDirs.append(path);
        persistState();
    }
}

bool BindRootManager::start(bool force)
{
    if (workDir.isEmpty() || !workDir.startsWith('/')) {
        qWarning() << "Invalid bind-root work dir:" << workDir;
        return false;
    }
    if (!force && QFileInfo::exists(workDir)) {
        qWarning() << "Bind-root work dir exists:" << workDir;
        return false;
    }
    shell.runAsRoot("rm -rf " + quoted(workDir), Cmd::QuietMode::Yes);
    shell.runAsRoot("mkdir -p " + quoted(workDir), Cmd::QuietMode::Yes);

    if (!shell.runAsRoot("mkdir -p " + quoted(bindRoot), Cmd::QuietMode::Yes)) {
        qWarning() << "Could not create bind-root dir:" << bindRoot;
        return false;
    }

    if (!shell.run("mountpoint -q " + quoted(bindRoot), Cmd::QuietMode::Yes)) {
        if (!shell.runAsRoot("mount --bind --make-slave " + quoted(realRoot) + " " + quoted(bindRoot))) {
            qWarning() << "Could not bind mount root to" << bindRoot;
            return false;
        }
    }

    rmDirs.clear();
    rmFiles.clear();
    return persistState();
}

bool BindRootManager::makeReadOnly()
{
    return shell.runAsRoot("mount -o remount,bind,ro " + quoted(bindRoot), Cmd::QuietMode::Yes);
}

bool BindRootManager::copyTemplateDir(const QString &source, const QString &dest)
{
    QDir().mkpath(dest);
    return shell.run("cp -r " + quoted(source) + "/. " + quoted(dest), Cmd::QuietMode::Yes);
}

bool BindRootManager::bindMountTemplate(const QString &templateDir)
{
    if (!QFileInfo::exists(templateDir)) {
        qWarning() << "Template dir missing:" << templateDir;
        return false;
    }

    QDir baseDir(templateDir);
    QDirIterator it(templateDir, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();
        const QString relative = baseDir.relativeFilePath(info.filePath());
        const QString base = "/" + relative;
        const QString target = bindRoot + base;
        const QString orig = realRoot + base;

        if (info.isSymLink()) {
            if (QFileInfo::exists(orig)) {
                continue;
            }
            const QString cmdCopy = "cp -a " + quoted(info.filePath()) + " " + quoted(orig);
            if (!shell.runAsRoot(cmdCopy)) {
                qWarning() << "Failed to copy symlink:" << info.filePath();
                continue;
            }
            const QFileInfo origInfo(orig);
            const QString targetLink = origInfo.symLinkTarget();
            if (!targetLink.isEmpty() && QFileInfo::exists(targetLink)) {
                addRmFile(orig);
            } else {
                shell.runAsRoot("rm -f " + quoted(orig), Cmd::QuietMode::Yes);
            }
            continue;
        }

        if (info.isDir()) {
            continue;
        }

        if (QFileInfo(orig).isSymLink()) {
            qWarning() << "Refusing to bind mount over symlink:" << orig;
            continue;
        }
        if (!touchFile(target, orig)) {
            qWarning() << "Failed to prepare target file:" << target;
            continue;
        }
        if (!shell.runAsRoot("mount --bind " + quoted(info.filePath()) + " " + quoted(target))) {
            qWarning() << "Failed to bind mount file:" << info.filePath();
            return false;
        }
    }
    return true;
}

bool BindRootManager::makeDir(const QString &targetDir, const QString &origDir)
{
    if (QFileInfo(targetDir).isDir()) {
        return true;
    }
    if (!QFileInfo::exists(origDir)) {
        addRmDir(origDir);
    }
    return shell.runAsRoot("mkdir -p " + quoted(targetDir));
}

bool BindRootManager::touchFile(const QString &targetFile, const QString &origFile)
{
    QFileInfo info(targetFile);
    if (info.exists()) {
        return info.isFile();
    }
    if (!makeDir(info.path(), QFileInfo(origFile).path())) {
        return false;
    }
    if (!QFileInfo::exists(origFile)) {
        addRmFile(origFile);
    }
    return shell.runAsRoot("touch " + quoted(targetFile));
}

bool BindRootManager::doLiveFiles()
{
    const QString source = "/usr/share/live-files/files";
    const QString dest = workDir + "/live-files";
    if (!copyTemplateDir(source, dest)) {
        return false;
    }
    return bindMountTemplate(dest);
}

bool BindRootManager::doGeneralFiles()
{
    const QString source = "/usr/share/live-files/general-files";
    const QString dest = workDir + "/general-files";
    if (!copyTemplateDir(source, dest)) {
        return false;
    }
    return bindMountTemplate(dest);
}

bool BindRootManager::doRepo()
{
    const QString repoDir = workDir + "/repo";
    const QString realDir = "/etc/apt/sources.list.d";
    const QString copyDir = repoDir + realDir;
    shell.runAsRoot("mkdir -p " + quoted(copyDir), Cmd::QuietMode::Yes);
    shell.runAsRoot("cp -a " + quoted(realDir) + "/* " + quoted(copyDir), Cmd::QuietMode::Yes);
    shell.runAsRoot("localize-repo --quiet --dir=" + quoted(copyDir) + " us", Cmd::QuietMode::Yes);
    return bindMountTemplate(repoDir);
}

QString BindRootManager::readFileRoot(const QString &path)
{
    return shell.getOutAsRoot("cat " + quoted(path) + " 2>/dev/null", Cmd::QuietMode::Yes);
}

bool BindRootManager::statFileRoot(const QString &path, QString &mode, QString &uid, QString &gid)
{
    const QString output = shell.getOutAsRoot("stat -c '%a %u %g' " + quoted(path), Cmd::QuietMode::Yes);
    const QStringList parts = output.split(' ', Qt::SkipEmptyParts);
    if (parts.size() != 3) {
        return false;
    }
    mode = parts.at(0);
    uid = parts.at(1);
    gid = parts.at(2);
    return true;
}

bool BindRootManager::writeFileRoot(const QString &path, const QString &content)
{
    QString mode;
    QString uid;
    QString gid;
    if (!statFileRoot(path, mode, uid, gid)) {
        return false;
    }

    QTemporaryFile temp(QDir::tempPath() + "/mx-snapshot-XXXXXX");
    if (!temp.open()) {
        return false;
    }
    QByteArray data = content.toUtf8();
    if (!data.endsWith('\n')) {
        data.append('\n');
    }
    if (temp.write(data) != data.size()) {
        return false;
    }
    temp.flush();

    const QString installCmd = "install -m " + mode + " -o " + uid + " -g " + gid + " "
        + quoted(temp.fileName()) + " " + quoted(path);
    return shell.runAsRoot(installCmd, Cmd::QuietMode::Yes);
}

bool BindRootManager::doTimezone()
{
    const QString tz = "America/New_York";
    const QString tzFile = "/etc/timezone";
    QString currentTz;
    QFile file(tzFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        currentTz = QString::fromUtf8(file.readAll()).trimmed();
    }
    if (currentTz == tz) {
        return true;
    }

    const QString zoneFile = "/usr/share/zoneinfo/" + tz;
    if (!QFileInfo::exists(zoneFile)) {
        qWarning() << "Timezone file missing:" << zoneFile;
        return false;
    }

    const QString tzDir = workDir + "/tz";
    const QString etcDir = tzDir + "/etc";
    QDir().mkpath(etcDir);

    QFile tzOut(etcDir + "/timezone");
    if (tzOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&tzOut);
        stream << tz << '\n';
    }

    const QString localtimePath = "/etc/localtime";
    if (!QFileInfo(localtimePath).isSymLink()) {
        shell.runAsRoot("cp -a " + quoted(localtimePath) + " " + quoted(etcDir + "/localtime"), Cmd::QuietMode::Yes);
    }

    return bindMountTemplate(tzDir);
}

bool BindRootManager::doAdjtime()
{
    const QString adjtime = "/etc/adjtime";
    if (!QFileInfo::exists(adjtime)) {
        return true;
    }

    QFile input(adjtime);
    if (!input.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    const QString content = QString::fromUtf8(input.readAll());
    QStringList lines = content.split('\n');
    if (lines.size() >= 1) {
        lines[0] = "0.0 0 0.0";
    }
    if (lines.size() >= 2) {
        lines[1] = "0";
    }
    const QString templateDir = workDir + "/adjtime";
    const QString targetFile = templateDir + "/etc/adjtime";
    QDir().mkpath(QFileInfo(targetFile).path());
    QFile output(targetFile);
    if (output.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&output);
        stream << lines.join('\n');
    }
    return bindMountTemplate(templateDir);
}

bool BindRootManager::doVersionFile(const QString &title)
{
    const QString versionDir = workDir + "/version-file";
    const QString versionFile = versionDir + "/etc/live/version/linuxfs.ver";
    QDir().mkpath(QFileInfo(versionFile).path());

    const QString existing = "/etc/live/version/linuxfs.ver";
    if (QFileInfo::exists(existing)) {
        shell.runAsRoot("cp -a " + quoted(existing) + " " + quoted(versionFile), Cmd::QuietMode::Yes);
    }

    QFile out(versionFile);
    if (!out.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }
    QTextStream stream(&out);
    stream << "==== " << randomHex32() << '\n';
    stream << "\n";
    stream << "title: " << title << '\n';
    stream << "creation date: " << QDateTime::currentDateTime().toString("d MMMM yyyy HH:mm:ss t") << '\n';
    stream << "kernel: " << shell.getOut("uname -sr", Cmd::QuietMode::Yes) << '\n';
    stream << "machine: " << shell.getOut("uname -m", Cmd::QuietMode::Yes) << '\n';
    return bindMountTemplate(versionDir);
}

bool BindRootManager::doBindMounts(const QStringList &dirs)
{
    for (const QString &dir : dirs) {
        if (dir.isEmpty()) {
            continue;
        }
        const QString trimmed = dir.startsWith('/') ? dir : "/" + dir;
        const QString realDir = realRoot + trimmed;
        const QString target = bindRoot + trimmed;
        if (!shell.run("mountpoint -q " + quoted(realDir), Cmd::QuietMode::Yes)) {
            continue;
        }
        if (!shell.runAsRoot("mount --bind " + quoted(realDir) + " " + quoted(target), Cmd::QuietMode::Yes)) {
            return false;
        }
    }
    return true;
}

bool BindRootManager::doEmptyDirs(const QStringList &dirs)
{
    const QString emptyDir = workDir + "/empty";
    for (const QString &dir : dirs) {
        if (dir.isEmpty()) {
            continue;
        }
        const QString trimmed = dir.startsWith('/') ? dir.mid(1) : dir;
        const QString target = bindRoot + "/" + trimmed;
        if (!QFileInfo(target).isDir()) {
            continue;
        }
        const QString origDir = realRoot + "/" + trimmed;
        if (!makeDir(target, origDir)) {
            return false;
        }
        const QString templateDir = emptyDir + "/" + trimmed;
        QDir().mkpath(templateDir);
        if (!shell.runAsRoot("mount --bind " + quoted(templateDir) + " " + quoted(target), Cmd::QuietMode::Yes)) {
            return false;
        }
    }
    return true;
}

bool BindRootManager::doPasswd()
{
    const QString templateDir = "/usr/share/live-files";
    const QString bindFrom = workDir + "/bind";
    const QString defUser = "demo";
    const QString defUserPw = "demo";
    const QString rootPw = "root";
    const QStringList bindFiles {
        "/etc/passwd",
        "/etc/shadow",
        "/etc/gshadow",
        "/etc/group",
        "/etc/subuid",
        "/etc/subgid"
    };

    QDir().mkpath(bindFrom);

    QString defShell = "/usr/bin/bash";
    QFile adduserConf("/etc/adduser.conf");
    if (adduserConf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&adduserConf);
        while (!stream.atEnd()) {
            const QString line = stream.readLine();
            if (line.startsWith("DSHELL=")) {
                defShell = line.section('=', 1).trimmed();
            }
        }
    }
    if (defShell.isEmpty()) {
        defShell = "/usr/bin/bash";
    }
    if (!defShell.startsWith('/')) {
        defShell = "/usr/bin/" + defShell;
    }

    QString commentOpt = "--gecos";
    const QString adduserHelp = shell.getOut("LC_ALL=C adduser --help 2>/dev/null", Cmd::QuietMode::Yes);
    if (adduserHelp.contains("--comment")) {
        commentOpt = "--comment";
    }

    const QString addedUser = "live_temp_user" + randomHex32().left(12);
    if (!shell.runAsRoot("adduser --disabled-login --shell " + quoted(defShell) + " --no-create-home "
                         + commentOpt + " " + defUser + " " + addedUser,
                         Cmd::QuietMode::Yes)) {
        qWarning() << "Failed to add temporary user.";
        return false;
    }
    const QString addedGroup = shell.getOutAsRoot("id -u " + quoted(addedUser), Cmd::QuietMode::Yes);

    QString srcRoot = bindRoot;
    const QString fsType = shell.getOut("findmnt -n -o FSTYPE " + quoted(bindRoot), Cmd::QuietMode::Yes);
    if (fsType.trimmed() == "overlay") {
        const QString passwdContent = shell.getOut("grep -q '^" + addedUser + ":' " + quoted(bindRoot + "/etc/passwd") + " && echo yes || echo no",
                                                   Cmd::QuietMode::Yes);
        if (passwdContent.trimmed() != "yes") {
            srcRoot = realRoot;
        }
    }

    for (const QString &file : bindFiles) {
        if (!QFileInfo::exists(bindRoot + file)) {
            continue;
        }
        if (QFileInfo::exists(templateDir + file)) {
            continue;
        }
        QDir().mkpath(bindFrom + QFileInfo(file).path());
        if (!shell.runAsRoot("cp -a " + quoted(srcRoot + file) + " " + quoted(bindFrom + file),
                             Cmd::QuietMode::Yes)) {
            qWarning() << "Failed to copy file for passwd munging:" << file;
            return false;
        }
    }

    shell.runAsRoot("deluser " + quoted(addedUser), Cmd::QuietMode::Yes);

    QStringList otherUsers;
    QFile passwdFile("/etc/passwd");
    if (passwdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&passwdFile);
        while (!stream.atEnd()) {
            const QString line = stream.readLine();
            if (line.trimmed().isEmpty()) {
                continue;
            }
            const QStringList fields = line.split(':');
            if (fields.size() < 7) {
                continue;
            }
            const QString user = fields.at(0);
            const QString home = fields.at(5);
            const QString shellField = fields.at(6);
            if (user == addedUser) {
                continue;
            }
            if (home.startsWith("/home/") && shellField.endsWith("sh")) {
                otherUsers.append(user);
            }
        }
    }

    const QStringList pwFiles {"/etc/passwd", "/etc/shadow", "/etc/gshadow", "/etc/subuid", "/etc/subgid"};
    const QStringList grpFiles {"/etc/group", "/etc/gshadow"};

    for (const QString &file : pwFiles) {
        const QString target = bindFrom + file;
        if (!QFileInfo::exists(target)) {
            continue;
        }
        QString content = readFileRoot(target);
        if (content.isEmpty()) {
            continue;
        }
        QStringList lines = content.split('\n');
        QStringList updated;
        updated.reserve(lines.size());
        for (const QString &line : lines) {
            if (line.isEmpty()) {
                continue;
            }
            QStringList fields = line.split(':');
            if (fields.isEmpty()) {
                continue;
            }
            const QString user = fields.at(0);
            if (otherUsers.contains(user)) {
                continue;
            }
            if (user == addedUser) {
                fields[0] = defUser;
            }
            if (file.endsWith("/passwd") && fields.size() >= 7 && fields.at(0) == defUser) {
                fields[2] = "1000";
                fields[3] = "1000";
                fields[5] = "/home/" + defUser;
            }
            updated.append(fields.join(':'));
        }
        if (!writeFileRoot(target, updated.join('\n') + "\n")) {
            return false;
        }
    }

    for (const QString &file : grpFiles) {
        const QString target = bindFrom + file;
        if (!QFileInfo::exists(target)) {
            continue;
        }
        QString content = readFileRoot(target);
        if (content.isEmpty()) {
            continue;
        }
        QStringList lines = content.split('\n');
        QStringList updated;
        updated.reserve(lines.size());
        for (const QString &line : lines) {
            if (line.isEmpty()) {
                continue;
            }
            QStringList fields = line.split(':');
            if (fields.size() < 4) {
                updated.append(line);
                continue;
            }
            auto sanitizeList = [&otherUsers, &addedUser, &defUser](const QString &input) {
                QStringList members = input.split(',', Qt::SkipEmptyParts);
                for (QString &member : members) {
                    if (member == addedUser) {
                        member = defUser;
                    }
                }
                if (!otherUsers.isEmpty()) {
                    members.erase(std::remove_if(members.begin(), members.end(),
                                                 [&otherUsers](const QString &member) {
                                                     return otherUsers.contains(member);
                                                 }),
                                  members.end());
                }
                return members.join(',');
            };

            if (file.endsWith("/gshadow")) {
                fields[2] = sanitizeList(fields.at(2));
                fields[3] = sanitizeList(fields.at(3));
            } else {
                fields[3] = sanitizeList(fields.at(3));
                if (fields.at(0) == defUser) {
                    fields[2] = "1000";
                }
            }
            updated.append(fields.join(':'));
        }
        if (!writeFileRoot(target, updated.join('\n') + "\n")) {
            return false;
        }
    }

    auto updateSubIdDefaults = [this, &bindFrom, defUser](const QString &fileName, const QString &defaultKey) {
        const QString target = bindFrom + fileName;
        if (!QFileInfo::exists(target)) {
            return true;
        }
        QString content = readFileRoot(target);
        if (content.isEmpty()) {
            return true;
        }
        QStringList lines = content.split('\n', Qt::SkipEmptyParts);
        if (lines.size() != 1) {
            return true;
        }
        QString defaultValue;
        QFile loginDefs("/etc/login.defs");
        if (loginDefs.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&loginDefs);
            while (!stream.atEnd()) {
                const QString line = stream.readLine().trimmed();
                if (line.startsWith(defaultKey + "=")) {
                    defaultValue = line.section('=', 1).trimmed();
                }
            }
        }
        if (defaultValue.isEmpty()) {
            defaultValue = "100000";
        }
        QString line = lines.first();
        if (line.startsWith(defUser + ":")) {
            const QStringList fields = line.split(':');
            if (fields.size() >= 3) {
                line = defUser + ":" + defaultValue + ":" + fields.at(2);
                if (!writeFileRoot(target, line + "\n")) {
                    return false;
                }
            }
        }
        return true;
    };

    if (!updateSubIdDefaults("/etc/subuid", "SUB_UID_MIN")) {
        return false;
    }
    if (!updateSubIdDefaults("/etc/subgid", "SUB_GID_MIN")) {
        return false;
    }

    const QString shadowFile = bindFrom + "/etc/shadow";
    if (QFileInfo::exists(shadowFile)) {
        const QString demoHash = sha512Crypt(defUserPw);
        const QString rootHash = sha512Crypt(rootPw);
        if (demoHash.isEmpty() || rootHash.isEmpty()) {
            qWarning() << "Failed to generate password hashes.";
            return false;
        }
        QString content = readFileRoot(shadowFile);
        QStringList lines = content.split('\n');
        for (QString &line : lines) {
            if (line.trimmed().isEmpty()) {
                continue;
            }
            QStringList fields = line.split(':');
            if (fields.size() < 2) {
                continue;
            }
            if (fields.at(0) == defUser) {
                fields[1] = demoHash.trimmed();
                line = fields.join(':');
            } else if (fields.at(0) == "root") {
                fields[1] = rootHash.trimmed();
                line = fields.join(':');
            }
        }
        if (!writeFileRoot(shadowFile, lines.join('\n') + "\n")) {
            return false;
        }
    }

    for (const QString &file : bindFiles) {
        const QString from = bindFrom + file;
        const QString to = bindRoot + file;
        const QString backup = to + "-";
        if (!QFileInfo::exists(from) || !QFileInfo::exists(to)) {
            continue;
        }
        if (!shell.runAsRoot("mount --bind " + quoted(from) + " " + quoted(to), Cmd::QuietMode::Yes)) {
            return false;
        }
        if (QFileInfo::exists(backup)) {
            shell.runAsRoot("mount --bind " + quoted(from) + " " + quoted(backup), Cmd::QuietMode::Yes);
        }
    }
    return true;
}

bool BindRootManager::doGeneral()
{
    if (!doEmptyDirs({"/etc/modprobe.d", "/etc/grub.d", "/etc/network/interfaces.d"})) {
        return false;
    }
    if (!doLiveFiles()) {
        return false;
    }
    if (!doGeneralFiles()) {
        return false;
    }
    if (!doPasswd()) {
        return false;
    }
    if (!doRepo()) {
        return false;
    }
    return doTimezone();
}

bool BindRootManager::cleanup()
{
    if (!stateLoaded && !loadState()) {
        return false;
    }

    const QString stateFile = stateFilePath();
    for (int i = 0; i < 10; ++i) {
        if (!shell.run("mountpoint -q " + quoted(bindRoot), Cmd::QuietMode::Yes)) {
            break;
        }
        shell.runAsRoot("umount --recursive " + quoted(bindRoot), Cmd::QuietMode::Yes);
        QThread::msleep(100);
    }
    if (shell.run("mountpoint -q " + quoted(bindRoot), Cmd::QuietMode::Yes)) {
        qWarning() << "Failed to unmount bind-root:" << bindRoot;
        return false;
    }

    shell.runAsRoot("rmdir " + quoted(bindRoot), Cmd::QuietMode::Yes);

    QStringList files = rmFiles;
    for (auto it = files.crbegin(); it != files.crend(); ++it) {
        shell.runAsRoot("rm -f " + quoted(*it), Cmd::QuietMode::Yes);
    }

    QStringList dirs = rmDirs;
    for (auto it = dirs.crbegin(); it != dirs.crend(); ++it) {
        shell.runAsRoot("rmdir --ignore-fail-on-non-empty --parents " + quoted(*it), Cmd::QuietMode::Yes);
    }

    shell.runAsRoot("rm -f " + quoted(stateFile), Cmd::QuietMode::Yes);
    shell.runAsRoot("rm -rf " + quoted(workDir), Cmd::QuietMode::Yes);
    return true;
}
