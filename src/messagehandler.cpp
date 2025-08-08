#include "messagehandler.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#ifndef CLI_BUILD
#include <QMessageBox>
#endif

bool MessageHandler::isGuiMode()
{
    // Check if we have a QApplication (GUI) vs QCoreApplication (CLI)
    return qApp && qApp->metaObject()->className() == QLatin1String("QApplication");
}

void MessageHandler::showMessage(MessageType type, const QString &title, const QString &message)
{
#ifndef CLI_BUILD
    if (isGuiMode()) {
        showGuiMessage(type, title, message);
        return;
    }
#endif
    showCliMessage(type, title, message);
}

void MessageHandler::showGuiMessage(MessageType type, const QString &title, const QString &message)
{
#ifndef CLI_BUILD
    QMessageBox::Icon icon;
    switch (type) {
    case Information:
        icon = QMessageBox::Information;
        break;
    case Warning:
        icon = QMessageBox::Warning;
        break;
    case Critical:
        icon = QMessageBox::Critical;
        break;
    }
    
    QMessageBox msgBox;
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.exec();
#else
    Q_UNUSED(type)
    Q_UNUSED(title)
    Q_UNUSED(message)
#endif
}

void MessageHandler::showCliMessage(MessageType type, const QString &title, const QString &message)
{
    QString prefix;
    switch (type) {
    case Information:
        prefix = "INFO";
        break;
    case Warning:
        prefix = "WARNING";
        break;
    case Critical:
        prefix = "ERROR";
        break;
    }
    
    if (!title.isEmpty()) {
        qDebug().noquote() << QString("[%1] %2: %3").arg(prefix, title, message);
    } else {
        qDebug().noquote() << QString("[%1] %2").arg(prefix, message);
    }
}