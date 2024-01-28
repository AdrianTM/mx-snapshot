/**********************************************************************
 *  batchprocessing.h
 **********************************************************************
 * Copyright (C) 2020-2024 MX Authors
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

#include <QTimer>

#include "settings.h"
#include "work.h"

class Batchprocessing : public QObject, public Settings
{
    Q_OBJECT
public:
    explicit Batchprocessing(const QCommandLineParser &arg_parser, QObject *parent = nullptr);

    void setConnections();

public slots:
    static void progress();

private:
    Work work;
    QTimer timer;
};
