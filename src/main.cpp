/**********************************************************************
 *  main.cpp
 **********************************************************************
 * Copyright (C) 2015-2025 MX Authors
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
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include <csignal>
#include <unistd.h>

#ifndef CLI_BUILD
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#endif

#include "batchprocessing.h"
#include "common.h"
#include "log.h"
#include "messagehandler.h"
#include "version.h"

static QTranslator qtTran, qtBaseTran, appTran;
inline QString current_kernel {};

void checkSquashfs();
void setTranslation();
void signalHandler(int signal);

int main(int argc, char *argv[])
{
    if (getuid() == 0) {
        qputenv("XDG_RUNTIME_DIR", "/run/user/0");
        qunsetenv("SESSION_MANAGER");
        qputenv("HOME", "/root");
    }

    const std::array<int, 3> signalList {SIGINT, SIGTERM, SIGHUP}; // allow SIGQUIT CTRL-\?
    for (auto signalName : signalList) {
        signal(signalName, signalHandler);
    }

    QProcess proc;
    proc.start("logname", {}, QIODevice::ReadOnly);
    proc.waitForFinished();
    const QString logname = QString::fromLatin1(proc.readAllStandardOutput().trimmed());

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Tool used for creating a live-CD from the running system"));
    parser.addHelpOption();
    parser.addVersionOption();
#ifndef CLI_BUILD
    parser.addOption({{"c", "cli"}, QObject::tr("Use CLI only")});
#endif

    const QVector<QCommandLineOption> options {
        {"cores", QObject::tr("Number of CPU cores to be used."), "number"},
        {{"d", "directory"}, QObject::tr("Output directory"), "path"},
        {{"f", "file"}, QObject::tr("Output filename"), "name"},
        {{"k", "kernel"},
         QObject::tr("Name a different kernel to use other than the default running kernel, use format returned by "
                     "'uname -r'")
             + " " + QObject::tr("Or the full path: %1").arg("/boot/vmlinuz-x.xx.x..."),
         "version, or path"},
        {{"l", "compression-level"},
         QObject::tr("Compression level options.") + " "
             + QObject::tr("Use quotes: \"-Xcompression-level <level>\", or \"-Xalgorithm <algorithm>\", or \"-Xhc\", "
                           "see mksquashfs man page"),
         "\"option\""},
        {{"m", "month"},
         QObject::tr("Create a monthly snapshot, add 'Month' name in the ISO name, skip used space calculation") + " "
             + QObject::tr("This option sets reset-accounts and compression to defaults, arguments changing those "
                           "items will be ignored") + " "
             + QObject::tr("Optionally specify a suffix to add to the month name (e.g., '1' for 'July.1')"), ""},
        {{"n", "no-checksums"}, QObject::tr("Don't calculate checksums for resulting ISO file"), ""},
        {{"o", "override-size"}, QObject::tr("Skip calculating free space to see if the resulting ISO will fit"), ""},
        {{"p", "preempt"}, QObject::tr("Option to fix issue with calculating checksums on preempt_rt kernels"), ""},
        {{"r", "reset"}, QObject::tr("Resetting accounts (for distribution to others)"), ""},
        {{"s", "checksums"}, QObject::tr("Calculate checksums for resulting ISO file"), ""},
        {{"t", "throttle"},
         QObject::tr("Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU "
                     "consumption of Mksquashfs."),
         "number"},
        {{"w", "workdir"}, QObject::tr("Work directory"), "path"},
        {{"x", "exclude"},
         QObject::tr("Exclude main folders, valid choices: ")
             + "Desktop, Documents, Downloads, Flatpaks, Music, Networks, Pictures, Steam, Videos, VirtualBox. "
             + QObject::tr("Use the option one time for each item you want to exclude"),
         "one item"},
        {{"z", "compression"},
         QObject::tr("Compression format, valid choices: ") + "lz4, lzo, gzip, xz, zstd",
         "format"},
        {"shutdown", QObject::tr("Shutdown computer when done.")}};

    for (const auto &option : options) {
        parser.addOption(option);
    }

    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setOrganizationName("MX-Linux");
    parser.process(QCoreApplication(argc, argv));
    const QString compressionValue = parser.value("compression");
    const QStringList allowedComp {"lz4", "lzo", "gzip", "xz", "zstd"};
    if (!compressionValue.isEmpty() && !allowedComp.contains(compressionValue)) {
        qDebug() << "Error: Unsupported compression format:" << compressionValue;
        qDebug() << "Supported formats:" << allowedComp.join(", ");
        qDebug() << "Please use one of the supported formats or omit the option to use default (zstd).";
        return EXIT_FAILURE;
    }

    QCoreApplication *app;
#ifdef CLI_BUILD
    app = new QCoreApplication(argc, argv);
#else
    // Determine if we should run in CLI mode based on multiple factors
    const bool forceCliMode = parser.isSet("cli") || parser.isSet("help") ||
                              QString(argv[0]).contains("cli") ||
                              !qEnvironmentVariableIsEmpty("MX_SNAPSHOT_CLI");

    if (forceCliMode) {
        app = new QCoreApplication(argc, argv);
    } else {
        // Set Qt platform to XCB (X11) if not already set and we're in X11 environment
        if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
            if (!qEnvironmentVariableIsEmpty("DISPLAY") && qEnvironmentVariableIsEmpty("WAYLAND_DISPLAY")) {
                qputenv("QT_QPA_PLATFORM", "xcb");
            }
        }
        app = new QApplication(argc, argv);
        QApplication::setApplicationDisplayName(QObject::tr("MX Snapshot"));
    }
#endif

    if (logname == "root") {
        const QString message = QObject::tr(
            "You seem to be logged in as root, please log out and log in as normal user to use this program.");
        MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message);
        return EXIT_FAILURE;
    }

    setTranslation();
    checkSquashfs();

    const bool isGuiApp = QCoreApplication::instance()->inherits("QApplication");
    const bool hasAuthTools = QFile::exists("/usr/bin/pkexec") || QFile::exists("/usr/bin/gksu");
    if (getuid() != 0 && (!isGuiApp || !hasAuthTools)) {
        qDebug().noquote() << QObject::tr("You must run this program with sudo or pkexec.");
        return EXIT_FAILURE;
    }

    const Log setLog("/tmp/" + app->applicationName() + ".log");
    qInstallMessageHandler(Log::messageHandler);
    qDebug().noquote() << app->applicationName() << QObject::tr("version:") << app->applicationVersion();
    if (argc > 1) {
        qDebug().noquote() << "Args:" << app->arguments();
    }

    // Create settings instance for dependency injection
    Settings settings(parser);

    if (!isGuiApp) {
        Batchprocessing batch(&settings);
        QTimer::singleShot(0, app, &QCoreApplication::quit);
        app->exec();
    }
#ifndef CLI_BUILD
    else {
        MainWindow w(&settings);
        w.show();
        app->exec();
    }
#endif
}

void setTranslation()
{
    const QString localeName = QLocale().name();
    const QString translationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    const QString appName = QCoreApplication::applicationName();

    if (qtTran.load("qt_" + localeName, translationsPath)) {
        QCoreApplication::installTranslator(&qtTran);
    }

    if (qtBaseTran.load("qtbase_" + localeName, translationsPath)) {
        QCoreApplication::installTranslator(&qtBaseTran);
    }

    if (appTran.load("mx-snapshot_" + localeName, "/usr/share/" + appName + "/locale")) {
        QCoreApplication::installTranslator(&appTran);
    }
}

void checkSquashfs()
{
    QProcess proc;
    proc.start("uname", {"-r"});
    proc.waitForFinished();
    current_kernel = proc.readAllStandardOutput().trimmed();

    const QString configPath = "/boot/config-" + current_kernel;
    if (QFile::exists(configPath) && QProcess::execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", configPath}) != 0) {
        const QString message = QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
        MessageHandler::showMessage(MessageHandler::Critical, QObject::tr("Error"), message);
        exit(EXIT_FAILURE);
    }
}

void signalHandler(int signal)
{
    const auto signame = strsignal(signal);
    qDebug() << "\nReceived signal:" << (signame ? signame : "Unknown signal");
    QCoreApplication::quit();
}
