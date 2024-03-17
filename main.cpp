/**********************************************************************
 *  main.cpp
 **********************************************************************
 * Copyright (C) 2015-2024 MX Authors
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
#ifdef CLI_BUILD
#include <QCoreApplication>
#else
#include <QApplication>
#endif
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include "batchprocessing.h"
#ifndef CLI_BUILD
#include "mainwindow.h"
#endif

#include "common.h"
#include "log.h"
#include "version.h"

#include <csignal>
#include <unistd.h>

static QTranslator qtTran, qtBaseTran, appTran;
inline QString current_kernel {};

void checkSquashfs();
void runApp(QCommandLineParser);
void setTranslation();
void signalHandler(int signal);

int main(int argc, char *argv[])
{
    if (getuid() == 0) {
        qputenv("XDG_RUNTIME_DIR", "/run/user/0");
        qunsetenv("SESSION_MANAGER");
    }
    const std::initializer_list<int> signalList {SIGINT, SIGTERM, SIGHUP}; // allow SIGQUIT CTRL-\?
    for (int signalName : signalList) {
        signal(signalName, signalHandler);
    }

    QProcess proc;
    proc.start("logname", {}, QIODevice::ReadOnly);
    proc.waitForFinished();
    auto const logname = QString::fromLatin1(proc.readAllStandardOutput().trimmed());

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Tool used for creating a live-CD from the running system"));
    parser.addHelpOption();
    parser.addVersionOption();
#ifndef CLI_BUILD
    parser.addOption({{"c", "cli"}, QObject::tr("Use CLI only")});
#endif
    parser.addOption({"cores", QObject::tr("Number of CPU cores to be used."), QObject::tr("number")});
    parser.addOption({{"d", "directory"}, QObject::tr("Output directory"), QObject::tr("path")});
    parser.addOption({{"f", "file"}, QObject::tr("Output filename"), QObject::tr("name")});
    parser.addOption({{"k", "kernel"},
                      QObject::tr("Name a different kernel to use other than the default running kernel, use format "
                                  "returned by 'uname -r'")
                          + " " + QObject::tr("Or the full path: %1").arg("/boot/vmlinuz-x.xx.x..."),
                      QObject::tr("version, or path")});
    parser.addOption({{"l", "compression-level"},
                      QObject::tr("Compression level options.") + " "
                          + QObject::tr("Use quotes: \"-Xcompression-level <level>\", "
                                        "or \"-Xalgorithm <algorithm>\", or \"-Xhc\", see mksquashfs man page"),
                      QObject::tr("\"option\"")});
    parser.addOption(
        {{"m", "month"},
         QObject::tr("Create a monthly snapshot, add 'Month' name in the ISO name, skip used space calculation") + " "
             + QObject::tr(
                 "This option sets reset-accounts and compression to defaults, arguments changing those items "
                 "will be ignored")});
    parser.addOption({{"n", "no-checksums"}, QObject::tr("Don't calculate checksums for resulting ISO file")});
    parser.addOption(
        {{"o", "override-size"}, QObject::tr("Skip calculating free space to see if the resulting ISO will fit")});
    parser.addOption(
        {{"p", "preempt"}, QObject::tr("Option to fix issue with calculating checksums on preempt_rt kernels")});
    parser.addOption({{"r", "reset"}, QObject::tr("Resetting accounts (for distribution to others)")});
    parser.addOption({{"s", "checksums"}, QObject::tr("Calculate checksums for resulting ISO file")});
    parser.addOption({{"t", "throttle"},
                      QObject::tr("Throttle the I/O input rate by the given percentage. This can be used to reduce the "
                                  "I/O and CPU consumption of Mksquashfs."),
                      QObject::tr("number")});
    parser.addOption({{"w", "workdir"}, QObject::tr("Work directory"), QObject::tr("path")});
    parser.addOption({{"x", "exclude"},
                      QObject::tr("Exclude main folders, valid choices: ")
                          + "Desktop, Documents, Downloads, Music, Networks, Pictures, Steam, Videos, VirtualBox. "
                          + QObject::tr("Use the option one time for each item you want to exclude"),
                      QObject::tr("one item")});
    parser.addOption({{"z", "compression"},
                      QObject::tr("Compression format, valid choices: ") + "lz4, lzo, gzip, xz, zstd",
                      QObject::tr("format")});
    parser.addOption({"shutdown", QObject::tr("Shutdown computer when done.")});

    QStringList opts;
    opts.reserve(argc);
    std::transform(argv, argv + argc, std::back_inserter(opts), [](const char *arg) { return QString::fromUtf8(arg); });
    parser.parse(opts);

    QStringList allowed_comp {"lz4", "lzo", "gzip", "xz", "zstd"};
    if (!parser.value("compression").isEmpty()) {
        if (!allowed_comp.contains(parser.value("compression"))) {
            qDebug() << "Wrong compression format";
            return EXIT_FAILURE;
        }
    }

#ifdef CLI_BUILD
    // Root guard
    if (logname == "root") {
        qDebug() << QObject::tr(
            "You seem to be logged in as root, please log out and log in as normal user to use this program.");
        exit(EXIT_FAILURE);
    }
    QCoreApplication app(argc, argv);
#else
    if (parser.isSet("cli") || parser.isSet("help")) {
        QCoreApplication app(argc, argv);
        // Root guard
        if (logname == "root") {
            qDebug() << QObject::tr(
                "You seem to be logged in as root, please log out and log in as normal user to use this program.");
            exit(EXIT_FAILURE);
        }
#endif
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setOrganizationName("MX-Linux");
    parser.process(app);
    setTranslation();
    checkSquashfs();
    if (getuid() == 0) {
        qputenv("HOME", "/root");
        Log setLog("/tmp/" + QCoreApplication::applicationName() + ".log");
        qDebug().noquote() << QCoreApplication::applicationName() << QObject::tr("version:")
                           << QCoreApplication::applicationVersion();
        if (argc > 1) {
            qDebug().noquote() << "Args:" << QCoreApplication::arguments();
        }
        Batchprocessing batch(parser);
        QTimer::singleShot(0, &app, &QCoreApplication::quit);
        return QCoreApplication::exec();
    } else {
        qDebug().noquote() << QObject::tr("You must run this program with sudo or pkexec.");
        return EXIT_FAILURE;
    }
#ifndef CLI_BUILD
}
else
{
    QApplication app(argc, argv);
    QApplication::setApplicationVersion(VERSION);
    QApplication::setOrganizationName("MX-Linux");
    QApplication::setApplicationDisplayName(QObject::tr("MX Snapshot"));
    parser.process(app);
    setTranslation();
    checkSquashfs();

    // Root guard
    if (logname == "root") {
        QMessageBox::critical(
            nullptr, QObject::tr("Error"),
            QObject::tr(
                "You seem to be logged in as root, please log out and log in as normal user to use this program."));
        exit(EXIT_FAILURE);
    }
    if (getuid() != 0) {
        if (!QFile::exists("/usr/bin/pkexec") && !QFile::exists("/usr/bin/gksu")) {
            QMessageBox::critical(nullptr, QObject::tr("Error"),
                                  QObject::tr("You must run this program with sudo or pkexec."));
            exit(EXIT_FAILURE);
        }
    }
    Log setLog("/tmp/" + QCoreApplication::applicationName() + ".log");
    qDebug().noquote() << QApplication::applicationName() << QObject::tr("version:")
                       << QApplication::applicationVersion();
    if (argc > 1) {
        qDebug().noquote() << "Args:" << QApplication::arguments();
    }
    MainWindow w(parser);
    w.show();
    auto const exit_code = QApplication::exec();
    proc.start("grep", {"^" + logname + ":", "/etc/passwd"});
    proc.waitForFinished();
    auto const home = QString::fromLatin1(proc.readAllStandardOutput().trimmed()).section(":", 5, 5);
    auto const file_name = home + "/.config/" + QApplication::applicationName() + "rc";
    if (QFile::exists(file_name)) {
        Cmd().runAsRoot("chown " + logname + ": " + file_name);
    }
    return exit_code;
}
#endif
}

void setTranslation()
{
    if (qtTran.load("qt_" + QLocale().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(&qtTran);
    }

    if (qtBaseTran.load("qtbase_" + QLocale().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(&qtBaseTran);
    }

    if (appTran.load("mx-snapshot_" + QLocale().name(),
                     "/usr/share/" + QCoreApplication::applicationName() + "/locale")) {
        QCoreApplication::installTranslator(&appTran);
    }
}

// Check if SQUASHFS is available
void checkSquashfs()
{
    QProcess proc;
    proc.start("uname", {"-r"});
    proc.waitForFinished();
    current_kernel = proc.readAllStandardOutput().trimmed();

    if (QFile::exists("/boot/config-" + current_kernel)
        && QProcess::execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", "/boot/config-" + current_kernel}) != 0) {
#ifdef CLI_BUILD
        qDebug() << QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
#else
            QString message = QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
            if (QCoreApplication::staticMetaObject.className() != QLatin1String("QApplication")) {
                qDebug() << message;
            } else {
                QMessageBox::critical(nullptr, QObject::tr("Error"), message);
            }
#endif
        exit(EXIT_FAILURE);
    }
}

void signalHandler(int signal)
{
    switch (signal) {
    case SIGHUP:
        qDebug() << "\nSIGHUP";
        break;
    case SIGINT:
        qDebug() << "\nSIGINT";
        break;
    case SIGQUIT:
        qDebug() << "\nSIGQUIT";
        break;
    case SIGTERM:
        qDebug() << "\nSIGTERM";
        break;
    }
    QCoreApplication::quit(); // Quit app anyway in case a subprocess was killed, but at least this calls aboutToQuit
}
