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
    static QString loggedInUserName();

    // Build a shell command string that invokes /usr/lib/<app>/snapshot-lib with
    // the given args, prefixed with whichever elevation tool fits the context.
    static QString snapshotLibCommand(const QString &args);
    static bool runSnapshotLib(const QString &args, QuietMode quiet = QuietMode::No);

    // A root operation failed because authentication was cancelled/denied (or the
    // privileged helper refused the command). Set by any Cmd instance; callers at
    // workflow entry points clear it first, then check it after the root calls to
    // decide whether to back out gracefully instead of aborting the application.
    static bool elevationDenied() { return s_elevationDenied; }
    static void clearElevationDenied() { s_elevationDenied = false; }

    bool proc(const QString &cmd, const QStringList &args = {}, QString *output = nullptr,
              const QByteArray *input = nullptr, QuietMode quiet = QuietMode::No,
              Elevation elevation = Elevation::No);
    bool procAsRoot(const QString &cmd, const QStringList &args = {}, QString *output = nullptr,
                    const QByteArray *input = nullptr, QuietMode quiet = QuietMode::No);
    // Invoke an action the privileged helper implements itself (as opposed to
    // the generic "exec <allow-listed command>" passthrough used by procAsRoot).
    // Authenticated the same way as procAsRoot: pkexec's grant is keyed on the
    // helper's exec path, so this requires admin auth regardless of action name.
    bool runHelperAction(const QString &action, const QStringList &args = {}, QString *output = nullptr,
                         QuietMode quiet = QuietMode::No);
    [[nodiscard]] QString getOut(const QString &cmd, QuietMode quiet = QuietMode::No);
    [[nodiscard]] QString getOutAsRoot(const QString &cmd, const QStringList &args, QuietMode quiet);
    // Shell-string overload: runs `bash -c "cmd"` as root.
    [[nodiscard]] QString getOutAsRoot(const QString &cmd, QuietMode quiet = QuietMode::No);
    [[nodiscard]] QString readAllOutput();
    bool run(const QString &cmd, QuietMode quiet = QuietMode::No);
    // Shell-string overload: runs `bash -c "cmd"` as root.
    bool runAsRoot(const QString &cmd, QuietMode quiet = QuietMode::No);

    // Suppress emission of outputAvailable/errorAvailable signals while still
    // buffering for readAllOutput(). Useful for noisy commands run inside other
    // commands' progress streams.
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
    inline static bool s_elevationDenied = false;
    // pkexec's own exit codes for a denied/cancelled authorization (see its man
    // page). The privileged helper (helper.cpp) is required to never return
    // either of these for its own internal errors (unknown command, allow-listed
    // binary missing, etc. all use a distinct code) so that a packaging or
    // allow-list bug is never misreported here as a cancelled authentication.
    static constexpr int EXIT_CODE_COMMAND_NOT_FOUND = 127;
    static constexpr int EXIT_CODE_PERMISSION_DENIED = 126;

    bool helperProc(const QStringList &helperArgs, QString *output = nullptr, const QByteArray *input = nullptr,
                    QuietMode quiet = QuietMode::No);
    [[nodiscard]] static QStringList helperExecArgs(const QString &cmd, const QStringList &args);
    void handleElevationError();
};
