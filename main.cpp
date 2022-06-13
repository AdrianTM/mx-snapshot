/**********************************************************************
 *  main.cpp
 **********************************************************************
 * Copyright (C) 2015 MX Authors
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
#include "version.h"
#include <csignal>
#include <unistd.h>

QString current_kernel;
static QFile logFile;
static QTranslator qtTran, qtBaseTran, appTran;

void checkSquashfs();
void messageHandler(QtMsgType, QMessageLogContext, QString);
void runApp(QCommandLineParser);
void setLog();
void setTranslation();
void signalHandler(int signal);

int main(int argc, char *argv[])
{
    qputenv("XDG_RUNTIME_DIR", "/run/user/0");
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
    //signal(SIGQUIT, signalHandler); // allow SIGQUIT CTRL-\?

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Tool used for creating a live-CD from the running system"));
    parser.addHelpOption();
    parser.addVersionOption();
#ifndef CLI_BUILD
    parser.addOption({{"c", "cli"}, QObject::tr("Use CLI only")});
#endif
    parser.addOption({{"d", "directory"}, QObject::tr("Output directory"), QObject::tr("path")});
    parser.addOption({{"f", "file"}, QObject::tr("Output filename"), QObject::tr("name")});
    parser.addOption({{"k", "kernel"}, QObject::tr("Name a different kernel to use other than the default running kernel, use format returned by 'uname -r'") + " " +
                      QObject::tr("Or the full path: %1").arg(QStringLiteral("/boot/vmlinuz-x.xx.x...")), QObject::tr("version, or path")});
    parser.addOption({{"l", "compression-level"}, QObject::tr("Compression level options.") + " "
                      + QObject::tr("Use quotes: \"-Xcompression-level <level>\", "
                      "or \"-Xalgorithm <algorithm>\", or \"-Xhc\", see mksquashfs man page"), QObject::tr("\"option\"")});
    parser.addOption({{"m", "month"}, QObject::tr("Create a monthly snapshot, add 'Month' name in the ISO name, skip used space calculation") + " " +
                      QObject::tr("This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored")});
    parser.addOption({{"n", "no-checksums"}, QObject::tr("Don't calculate checksums for resulting ISO file")});
    parser.addOption({{"p", "preempt"}, QObject::tr("Option to fix issue with calculating checksums on preempt_rt kernels")});
    parser.addOption({{"r", "reset"}, QObject::tr("Resetting accounts (for distribution to others)")});
    parser.addOption({{"s", "checksums"}, QObject::tr("Calculate checksums for resulting ISO file")});
    parser.addOption({{"o", "override-size"}, QObject::tr("Skip calculating free space to see if the resulting ISO will fit")});
    parser.addOption({{"x", "exclude"}, QObject::tr("Exclude main folders, valid choices: ")
                      + "Desktop, Documents, Downloads, Music, Networks, Pictures, Steam, Videos, VirtualBox. " +
                      QObject::tr("Use the option one time for each item you want to exclude"), QObject::tr("one item")});
    parser.addOption({{"z", "compression"}, QObject::tr("Compression format, valid choices: ")
                      + "lz4, lzo, gzip, xz, zstd", QObject::tr("format")});

    QStringList opts;
    opts.reserve(argc);
    for (int i = 0; i < argc; ++i)
        opts << QString(argv[i]);
    parser.parse(opts);

    QStringList allowed_comp {"lz4", "lzo", "gzip", "xz", "zstd"};
    if (!parser.value(QStringLiteral("compression")).isEmpty()) {
        if (!allowed_comp.contains(parser.value(QStringLiteral("compression")))) {
            qDebug() << "Wrong compression format";
            return EXIT_FAILURE;
        }
    }

#ifdef CLI_BUILD
    // root guard
    if (QProcess::execute("/bin/bash", {"-c", "logname |grep -q ^root$"}) == 0) {
        qDebug() << QObject::tr("You seem to be logged in as root, please log out and log in as normal user to use this program.");
        exit(EXIT_FAILURE);
    }
    QCoreApplication app(argc, argv);
#else
    if (parser.isSet(QStringLiteral("cli")) || parser.isSet(QStringLiteral("help"))) {
        QCoreApplication app(argc, argv);
        // root guard
        if (QProcess::execute("/bin/bash", {"-c", "logname |grep -q ^root$"}) == 0) {
            qDebug() << QObject::tr("You seem to be logged in as root, please log out and log in as normal user to use this program.");
            exit(EXIT_FAILURE);
        }
#endif
        app.setApplicationVersion(VERSION);
        parser.process(app);
        setTranslation();
        checkSquashfs();
        if (getuid() == 0) {
            setLog();
            qDebug().noquote() << qApp->applicationName() << QObject::tr("version:") << qApp->applicationVersion();
            if (argc > 1) qDebug().noquote() << "Args:" << qApp->arguments();
            Batchprocessing batch(parser);
            QTimer::singleShot(0, &app, &QCoreApplication::quit);
            return app.exec();
        } else {
            qDebug().noquote() << QObject::tr("You must run this program as root.");
            return EXIT_FAILURE;
        }
#ifndef CLI_BUILD
    } else {
        QApplication app(argc, argv);
        app.setApplicationVersion(VERSION);
        parser.process(app);
        setTranslation();
        checkSquashfs();

        // root guard
        if (QProcess::execute("/bin/bash", {"-c", "logname |grep -q ^root$"}) == 0) {
            QMessageBox::critical(nullptr, QObject::tr("Error"),
                                  QObject::tr("You seem to be logged in as root, please log out and log in as normal user to use this program."));
            exit(EXIT_FAILURE);
        }

        if (getuid() == 0) {
            setLog();
            qDebug().noquote() << qApp->applicationName() << QObject::tr("version:") << qApp->applicationVersion();
            if (argc > 1) qDebug().noquote() << "Args:" << qApp->arguments();
            qputenv("HOME", "/root");
            MainWindow w(nullptr, parser);
            w.show();
            exit(app.exec());
        } else {
            QProcess::startDetached("/usr/bin/mx-snapshot-launcher", {});
        }
    }
#endif
}


// The implementation of the handler
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream term_out(stdout);
    msg.contains(QLatin1String("\r")) ? term_out << msg : term_out << msg << "\n";

    if (msg.startsWith(QLatin1String("\033[2KProcessing")))
        return;
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz "));
    switch (type)
    {
    case QtInfoMsg:     out << QStringLiteral("INF "); break;
    case QtDebugMsg:    out << QStringLiteral("DBG "); break;
    case QtWarningMsg:  out << QStringLiteral("WRN "); break;
    case QtCriticalMsg: out << QStringLiteral("CRT "); break;
    case QtFatalMsg:    out << QStringLiteral("FTL "); break;
    }
    out << context.category << QStringLiteral(": ") << msg << "\n";
}

void setTranslation()
{
    if (qtTran.load(QLocale::system(), QStringLiteral("qt_"), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtTran);

    if (qtBaseTran.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtBaseTran);

    if (appTran.load(qApp->applicationName() + "_" + QLocale::system().name(), "/usr/share/"
                     + qApp->applicationName() + "/locale"))
        qApp->installTranslator(&appTran);
}

// Check if SQUASHFS is available
void checkSquashfs()
{
    QProcess proc;
    proc.start("uname", {"-r"}, QIODevice::ReadOnly);
    proc.waitForFinished();
    current_kernel = proc.readAllStandardOutput().trimmed();

    if (QFile::exists("/boot/config-" + current_kernel)
            && proc.execute("grep", {"-q", "^CONFIG_SQUASHFS=[ym]", "/boot/config-" + current_kernel}) != 0) {
#ifdef CLI_BUILD
        qDebug() << QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
#else
        QString message = QObject::tr("Current kernel doesn't support Squashfs, cannot continue.");
        if (qApp->metaObject()->className() !=  QLatin1String("QApplication"))
            qDebug() << message;
        else
            QMessageBox::critical(nullptr, QObject::tr("Error"), message);
#endif
        exit(EXIT_FAILURE);
    }
}

void setLog()
{
    QString log_name = "/var/log/" + qApp->applicationName() + ".log";
    if (QFileInfo::exists(log_name)) {
        QFile::remove(log_name + ".old");
        QFile::rename(log_name, log_name + ".old");
    }
    logFile.setFileName(log_name);
    logFile.open(QFile::Append | QFile::Text);
    qInstallMessageHandler(messageHandler);
}

void signalHandler(int signal)
{
    switch (signal)
    {
    case SIGHUP:  qDebug() << "\nSIGHUP"; break;
    case SIGINT:  qDebug() << "\nSIGINT"; break;
    case SIGQUIT: qDebug() << "\nSIGQUIT"; break;
    case SIGTERM: qDebug() << "\nSIGTERM"; break;
    }
    qApp->quit(); // quit app anyway in case a subprocess was killed, but at least this calls aboutToQuit
}
