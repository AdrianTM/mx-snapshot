#pragma once

#include <QtCore/QString>
#include <QtCore/QStringList>

class SystemInfo
{
public:
    // Check if running on 32-bit architecture
    [[nodiscard]] static bool is386();
    
    // Check if running from a live environment
    [[nodiscard]] static bool isLive();
    
    // Get list of users with home directories
    [[nodiscard]] static QStringList listUsers();
    
    // Read kernel boot options from script
    [[nodiscard]] static QString readKernelOpts();
};