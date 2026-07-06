/**********************************************************************
 *  elevationbroker.h
 **********************************************************************
 * Copyright (C) 2026 MX Authors
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
#pragma once

#include <QHash>
#include <QList>
#include <QObject>
#include <QProcess>

#include <functional>

// Client side of the helper's serve mode: one authenticated root helper
// (`pkexec <helper> serve`) shared by the whole process. It is launched
// lazily on the first elevated command — that is when the pkexec prompt
// appears — and then stays alive until the application exits, so polkit's
// ~5-minute auth_admin_keep window cannot expire between privileged steps
// (mksquashfs alone outlasts it). See helper.cpp for the wire protocol.
class ElevationBroker : public QObject
{
    Q_OBJECT
public:
    using OutputSink = std::function<void(const QByteArray &)>;

    enum class Launch { Ready, Denied, Failed };

    static ElevationBroker &instance();
    ~ElevationBroker() override;

    // Start (or confirm) the broker. Blocks — pumping the event loop, so the
    // GUI stays responsive behind the pkexec dialog — until the broker prints
    // READY or exits. Denied = authentication cancelled/refused (pkexec 126/127).
    Launch ensureStarted(const QString &helperPath, const QString &elevationTool);

    // Run one helper request (argv exactly as passed to the helper in one-shot
    // mode, e.g. {"exec", "mksquashfs", ...}). Streams child stdout / broker
    // stderr into the sinks while pumping the event loop. Returns the request's
    // exit code, or -1 if the broker died mid-request (the next ensureStarted()
    // relaunches it, which re-prompts once).
    int execute(const QStringList &helperArgs, const QByteArray &input, const OutputSink &onOutput,
                const OutputSink &onError);

    // Out-of-band: terminate the currently running child's process group
    // (e.g. mksquashfs on cancel). Queued requests still run afterwards.
    void killActiveChild();
    void shutdown();

    [[nodiscard]] bool isReady() const;

private:
    explicit ElevationBroker(QObject *parent = nullptr);

    struct Request
    {
        bool done = false;
        int code = -1;
        OutputSink onOutput;
        OutputSink onError;
        class QEventLoop *loop = nullptr;
    };

    void onStdout();
    void onStderr();
    void onFinished();
    void completeRequest(quint64 id, int code);
    void failAllPending();
    void wakeLaunchWaiters();

    QProcess proc;
    QByteArray buffer;
    QHash<quint64, Request *> pending;
    QList<quint64> order; // FIFO: front = oldest outstanding = currently running
    QList<class QEventLoop *> launchWaiters;
    quint64 nextId = 1;
    bool ready = false;
    bool shuttingDown = false;
    // O-frame payload currently being streamed
    quint64 frameId = 0;
    qint64 frameRemaining = -1;
};
