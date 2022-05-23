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

#-------------------------------------------------
#
# Project created by QtCreator 2014-12-14
#
#-------------------------------------------------

DEFINES += "CLI_BUILD=1"

QT       += core
QT       -= gui

CONFIG   += c++17

TARGET = iso-snapshot-cli
TEMPLATE = app


SOURCES += main.cpp \
    cmd.cpp \
    settings.cpp \
    batchprocessing.cpp \
    work.cpp

HEADERS  += \
    version.h \
    cmd.h \
    settings.h \
    batchprocessing.h \
    work.h

TRANSLATIONS += translations/iso-snapshot-cli_am.ts \
                translations/iso-snapshot-cli_ar.ts \
                translations/iso-snapshot-cli_bg.ts \
                translations/iso-snapshot-cli_bn.ts \
                translations/iso-snapshot-cli_ca.ts \
                translations/iso-snapshot-cli_cs.ts \
                translations/iso-snapshot-cli_da.ts \
                translations/iso-snapshot-cli_de.ts \
                translations/iso-snapshot-cli_el.ts \
                translations/iso-snapshot-cli_es.ts \
                translations/iso-snapshot-cli_et.ts \
                translations/iso-snapshot-cli_eu.ts \
                translations/iso-snapshot-cli_fa.ts \
                translations/iso-snapshot-cli_fi.ts \
                translations/iso-snapshot-cli_fil_PH.ts \
                translations/iso-snapshot-cli_fr.ts \
                translations/iso-snapshot-cli_he_IL.ts \
                translations/iso-snapshot-cli_hi.ts \
                translations/iso-snapshot-cli_hr.ts \
                translations/iso-snapshot-cli_hu.ts \
                translations/iso-snapshot-cli_id.ts \
                translations/iso-snapshot-cli_is.ts \
                translations/iso-snapshot-cli_it.ts \
                translations/iso-snapshot-cli_ja.ts \
                translations/iso-snapshot-cli_kk.ts \
                translations/iso-snapshot-cli_ko.ts \
                translations/iso-snapshot-cli_lt.ts \
                translations/iso-snapshot-cli_mk.ts \
                translations/iso-snapshot-cli_mr.ts \
                translations/iso-snapshot-cli_nb.ts \
                translations/iso-snapshot-cli_nl.ts \
                translations/iso-snapshot-cli_pl.ts \
                translations/iso-snapshot-cli_pt.ts \
                translations/iso-snapshot-cli_pt_BR.ts \
                translations/iso-snapshot-cli_ro.ts \
                translations/iso-snapshot-cli_ru.ts \
                translations/iso-snapshot-cli_sk.ts \
                translations/iso-snapshot-cli_sl.ts \
                translations/iso-snapshot-cli_sq.ts \
                translations/iso-snapshot-cli_sr.ts \
                translations/iso-snapshot-cli_sv.ts \
                translations/iso-snapshot-cli_tr.ts \
                translations/iso-snapshot-cli_uk.ts \
                translations/iso-snapshot-cli_vi.ts \
                translations/iso-snapshot-cli_zh_CN.ts \
                translations/iso-snapshot-cli_zh_TW.ts

