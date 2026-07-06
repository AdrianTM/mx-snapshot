/**********************************************************************
 *  elevationbroker.cpp
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
#include "elevationbroker.h"

#include <QDebug>
#include <QEventLoop>

ElevationBroker &ElevationBroker::instance()
{
    static ElevationBroker broker;
    return broker;
}

ElevationBroker::ElevationBroker(QObject *parent)
    : QObject(parent)
{
    connect(&proc, &QProcess::readyReadStandardOutput, this, &ElevationBroker::onStdout);
    connect(&proc, &QProcess::readyReadStandardError, this, &ElevationBroker::onStderr);
    connect(&proc, &QProcess::finished, this, &ElevationBroker::onFinished);
}

bool ElevationBroker::isReady() const
{
    return ready && proc.state() == QProcess::Running;
}

ElevationBroker::Launch ElevationBroker::ensureStarted(const QString &helperPath, const QString &elevationTool)
{
    if (isReady()) {
        return Launch::Ready;
    }
    if (proc.state() == QProcess::NotRunning) {
        ready = false;
        buffer.clear();
        frameRemaining = -1;
        qDebug() << "Starting elevation broker:" << elevationTool << helperPath << "serve";
        proc.start(elevationTool, {helperPath, QStringLiteral("serve")});
        if (!proc.waitForStarted()) {
            qWarning() << "Could not start elevation broker:" << proc.errorString();
            return Launch::Failed;
        }
    }
    // Wait for READY or exit. The authentication dialog may stay open for as
    // long as the user needs; pumping a local event loop keeps the UI alive.
    // The re-check before exec() is safe: signals are only delivered while an
    // event loop runs, so state cannot change between the check and exec().
    while (!ready && proc.state() == QProcess::Running) {
        QEventLoop wait;
        launchWaiters.append(&wait);
        wait.exec();
        launchWaiters.removeOne(&wait);
    }
    if (ready) {
        return Launch::Ready;
    }
    const int code = proc.exitCode();
    if (proc.exitStatus() == QProcess::NormalExit && (code == 126 || code == 127)) {
        return Launch::Denied; // pkexec: authentication cancelled / not authorized
    }
    qWarning() << "Elevation broker exited during startup, code" << code;
    return Launch::Failed;
}

int ElevationBroker::execute(const QStringList &helperArgs, const QByteArray &input, const OutputSink &onOutput,
                             const OutputSink &onError)
{
    if (!isReady()) {
        return -1;
    }
    const quint64 id = nextId++;
    Request req;
    req.onOutput = onOutput;
    req.onError = onError;
    pending.insert(id, &req);
    order.append(id);

    QByteArray msg = "REQ " + QByteArray::number(id) + ' ' + QByteArray::number(helperArgs.size()) + ' '
                     + QByteArray::number(input.size()) + '\n';
    for (const QString &arg : helperArgs) {
        const QByteArray bytes = arg.toUtf8();
        msg += QByteArray::number(bytes.size()) + '\n' + bytes;
    }
    msg += input;
    proc.write(msg);

    if (!req.done) {
        QEventLoop wait;
        req.loop = &wait;
        wait.exec();
        req.loop = nullptr;
    }
    pending.remove(id);
    order.removeOne(id);
    return req.code;
}

void ElevationBroker::killActiveChild()
{
    if (proc.state() == QProcess::Running) {
        proc.write("KILL\n");
    }
}

void ElevationBroker::onStdout()
{
    buffer += proc.readAllStandardOutput();
    while (true) {
        if (frameRemaining > 0) {
            if (buffer.isEmpty()) {
                return;
            }
            const qint64 take = qMin<qint64>(frameRemaining, buffer.size());
            const QByteArray chunk = buffer.left(take);
            buffer.remove(0, take);
            frameRemaining -= take;
            if (Request *req = pending.value(frameId, nullptr); req != nullptr && req->onOutput) {
                req->onOutput(chunk);
            }
            continue;
        }
        const qsizetype nl = buffer.indexOf('\n');
        if (nl < 0) {
            return;
        }
        const QByteArray line = buffer.left(nl);
        buffer.remove(0, nl + 1);
        const QList<QByteArray> parts = line.split(' ');
        if (parts.value(0) == "READY") {
            ready = true;
            wakeLaunchWaiters();
            continue;
        }
        if (parts.value(0) == "O" && parts.size() == 3) {
            frameId = parts.at(1).toULongLong();
            frameRemaining = parts.at(2).toLongLong();
            continue;
        }
        if (parts.value(0) == "R" && parts.size() == 3) {
            completeRequest(parts.at(1).toULongLong(), parts.at(2).toInt());
            continue;
        }
        qWarning() << "Elevation broker: unexpected frame:" << line;
    }
}

void ElevationBroker::onStderr()
{
    const QByteArray chunk = proc.readAllStandardError();
    if (chunk.isEmpty()) {
        return;
    }
    // stderr is untagged; the broker runs requests FIFO, so it belongs to the
    // oldest outstanding request. With none outstanding, just log it.
    if (!order.isEmpty()) {
        if (Request *req = pending.value(order.first(), nullptr); req != nullptr && req->onError) {
            req->onError(chunk);
            return;
        }
    }
    qWarning().noquote() << "Elevation broker:" << QString::fromLocal8Bit(chunk).trimmed();
}

void ElevationBroker::onFinished()
{
    ready = false;
    failAllPending();
    wakeLaunchWaiters();
}

void ElevationBroker::completeRequest(quint64 id, int code)
{
    Request *req = pending.value(id, nullptr);
    if (req == nullptr) {
        return;
    }
    req->done = true;
    req->code = code;
    if (req->loop != nullptr) {
        req->loop->quit();
    }
}

void ElevationBroker::failAllPending()
{
    const QList<quint64> ids = order;
    for (const quint64 id : ids) {
        completeRequest(id, -1);
    }
}

void ElevationBroker::wakeLaunchWaiters()
{
    const QList<QEventLoop *> waiters = launchWaiters;
    for (QEventLoop *loop : waiters) {
        loop->quit();
    }
}
