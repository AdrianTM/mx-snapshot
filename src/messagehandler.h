#pragma once

#include <QtCore/QString>

class MessageHandler
{
public:
    enum MessageType {
        Information,
        Warning,
        Critical
    };

    // Show message appropriate for current application type (GUI or CLI)
    static void showMessage(MessageType type, const QString &title, const QString &message);
    
    // Check if we're running in GUI mode
    [[nodiscard]] static bool isGuiMode();
    
private:
    // Show GUI message box (only available in GUI builds)
    static void showGuiMessage(MessageType type, const QString &title, const QString &message);
    
    // Show CLI message to console
    static void showCliMessage(MessageType type, const QString &title, const QString &message);
};