/**********************************************************************
 *
 **********************************************************************
 * Copyright (C) 2023-2025 MX Authors
 *
 * Authors: Adrian <adrian@mxlinux.org>
 *          MX Linux <http://mxlinux.org>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package. If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/
#include "about.h"

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>

#include "common.h"
#include <unistd.h>

// Display doc as nomal user when run as root
void displayDoc(const QString &url, const QString &title)
{
    const bool startedAsRoot = (qEnvironmentVariable("HOME") == "root");
    if (startedAsRoot) {
        qputenv("HOME", startingHome.toUtf8()); // Use original home for theming purposes
    }
    // Prefer mx-viewer otherwise use xdg-open (use runuser to run that as logname user)
    const QString executablePath = QStandardPaths::findExecutable("mx-viewer");
    if (!executablePath.isEmpty()) {
        QProcess::startDetached("mx-viewer", {url, title});
    } else {
        if (getuid() != 0) {
            QProcess::startDetached("xdg-open", {url});
        } else {
            QProcess proc;
            proc.start("logname", {}, QIODevice::ReadOnly);
            proc.waitForFinished();
            const QString user = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
            QProcess::startDetached("runuser", {"-u", user, "--", "xdg-open", url});
        }
    }
    if (startedAsRoot) {
        qputenv("HOME", "/root");
    }
}

void displayAboutMsgBox(const QString &title, const QString &message, const QString &licenseUrl,
                        const QString &licenseTitle)
{
    const auto width = 600;
    const auto height = 500;
    QMessageBox msgBox(QMessageBox::NoIcon, title, message);
    auto *btnLicense = msgBox.addButton(QObject::tr("License"), QMessageBox::HelpRole);
    auto *btnChangelog = msgBox.addButton(QObject::tr("Changelog"), QMessageBox::HelpRole);
    auto *btnCancel = msgBox.addButton(QObject::tr("Cancel"), QMessageBox::NoRole);
    btnCancel->setIcon(QIcon::fromTheme("window-close"));

    msgBox.exec();

    if (msgBox.clickedButton() == btnLicense) {
        displayDoc(licenseUrl, licenseTitle);
    } else if (msgBox.clickedButton() == btnChangelog) {
        QScopedPointer<QDialog> changelog(new QDialog);
        changelog->setWindowTitle(QObject::tr("Changelog"));
        changelog->resize(width, height);

        auto *text = new QTextEdit(changelog.data());
        text->setReadOnly(true);
        QProcess proc;
        proc.start(
            "zless",
            {"/usr/share/doc/" + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "/changelog.gz"},
            QIODevice::ReadOnly);
        proc.waitForFinished();
        text->setText(proc.readAllStandardOutput());

        auto *btnClose = new QPushButton(QObject::tr("&Close"), changelog.data());
        btnClose->setIcon(QIcon::fromTheme("window-close"));
        QObject::connect(btnClose, &QPushButton::clicked, changelog.data(), &QDialog::close);

        auto *layout = new QVBoxLayout(changelog.data());
        layout->addWidget(text);
        layout->addWidget(btnClose);
        changelog->setLayout(layout);
        changelog->exec();
    }
}
