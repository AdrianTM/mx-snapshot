#pragma once

#include <QProcess>

class QTextStream;

class Cmd : public QProcess
{
    Q_OBJECT
public:
    explicit Cmd(QObject *parent = nullptr);

    [[nodiscard]] QString getOut(const QString &cmd, bool quiet = false, bool asRoot = false);
    [[nodiscard]] QString getOutAsRoot(const QString &cmd, bool quiet = false);
    [[nodiscard]] QString readAllOutput();
    bool run(const QString &cmd, bool quiet = false, bool asRoot = false);
    bool runAsRoot(const QString &cmd, bool quiet = false);

signals:
    void done();
    void errorAvailable(const QString &err);
    void outputAvailable(const QString &out);

private:
    QString elevate;
    QString helper;
    QString out_buffer;
};
