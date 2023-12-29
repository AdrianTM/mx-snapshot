# **********************************************************************
# * Copyright (C) 2015 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This file is part of MX Snapshot.
# *
# * MX Snapshot is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * MX Snapshot is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with MX Tools.  If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************

DEFINES += "CLI_BUILD=1"

QT       += core
QT       -= gui

CONFIG   += c++1z

TARGET = iso-snapshot-cli
TEMPLATE = app

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    log.cpp \
    main.cpp \
    cmd.cpp \
    settings.cpp \
    batchprocessing.cpp \
    work.cpp

HEADERS  += \
    common.h \
    log.h \
    version.h \
    cmd.h \
    settings.h \
    batchprocessing.h \
    work.h

TRANSLATIONS += \
    translations/mx-snapshot_en.ts

RESOURCES += \
    images.qrc
