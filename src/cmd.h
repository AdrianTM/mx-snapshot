#pragma once

#include <QProcess>

class QTextStream;

class Cmd : public QProcess
{
    Q_OBJECT
public:
    explicit Cmd(QObject *parent = nullptr);

    enum class QuietMode { No, Yes };
    enum class Elevation { No, Yes };

    // Returns the elevation tool path appropriate for current mode (CLI/GUI),
    // preferring sudo in CLI, pkexec in GUI. Empty if none found.
    static QString elevationTool();
    static bool isCliMode();

    [[nodiscard]] QString getOut(const QString &cmd, QuietMode quiet = QuietMode::No, Elevation elevation = Elevation::No);
    [[nodiscard]] QString getOutAsRoot(const QString &cmd, QuietMode quiet = QuietMode::No);
    [[nodiscard]] QString readAllOutput();
    bool run(const QString &cmd, QuietMode quiet = QuietMode::No, Elevation elevation = Elevation::No);
    bool runAsRoot(const QString &cmd, QuietMode quiet = QuietMode::No);
    void setOutputSuppressed(bool suppressed);
    [[nodiscard]] bool outputSuppressed() const;

signals:
    void done();
    void errorAvailable(const QString &err);
    void outputAvailable(const QString &out);

private:
    const QString elevationToolPath;
    const QString helperPath;
    QString outBuffer;
    bool suppressOutput = false;
    static constexpr int EXIT_CODE_COMMAND_NOT_FOUND = 127;
    static constexpr int EXIT_CODE_PERMISSION_DENIED = 126;

    void handleElevationError();
};
