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

QT       += core gui widgets
CONFIG   += c++1z

TARGET = mx-snapshot
TEMPLATE = app

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    mainwindow.cpp \
    about.cpp \
    cmd.cpp \
    settings.cpp \
    batchprocessing.cpp \
    work.cpp

HEADERS  += \
    mainwindow.h \
    version.h \
    about.h \
    cmd.h \
    settings.h \
    batchprocessing.h \
    work.h

FORMS    += \
    mainwindow.ui

TRANSLATIONS += translations/mx-snapshot_af.ts \
                translations/mx-snapshot_am.ts \
                translations/mx-snapshot_ar.ts \
                translations/mx-snapshot_ast.ts \
                translations/mx-snapshot_be.ts \
                translations/mx-snapshot_bg.ts \
                translations/mx-snapshot_bn.ts \
                translations/mx-snapshot_bs_BA.ts \
                translations/mx-snapshot_bs.ts \
                translations/mx-snapshot_ca.ts \
                translations/mx-snapshot_ceb.ts \
                translations/mx-snapshot_co.ts \
                translations/mx-snapshot_cs.ts \
                translations/mx-snapshot_cy.ts \
                translations/mx-snapshot_da.ts \
                translations/mx-snapshot_de.ts \
                translations/mx-snapshot_el.ts \
                translations/mx-snapshot_en_GB.ts \
                translations/mx-snapshot_en_US.ts \
                translations/mx-snapshot_eo.ts \
                translations/mx-snapshot_es_ES.ts \
                translations/mx-snapshot_es.ts \
                translations/mx-snapshot_et.ts \
                translations/mx-snapshot_eu.ts \
                translations/mx-snapshot_fa.ts \
                translations/mx-snapshot_fi_FI.ts \
                translations/mx-snapshot_fil_PH.ts \
                translations/mx-snapshot_fil.ts \
                translations/mx-snapshot_fi.ts \
                translations/mx-snapshot_fr_BE.ts \
                translations/mx-snapshot_fr.ts \
                translations/mx-snapshot_fy.ts \
                translations/mx-snapshot_ga.ts \
                translations/mx-snapshot_gd.ts \
                translations/mx-snapshot_gl_ES.ts \
                translations/mx-snapshot_gl.ts \
                translations/mx-snapshot_gu.ts \
                translations/mx-snapshot_ha.ts \
                translations/mx-snapshot_haw.ts \
                translations/mx-snapshot_he_IL.ts \
                translations/mx-snapshot_he.ts \
                translations/mx-snapshot_hi.ts \
                translations/mx-snapshot_hr.ts \
                translations/mx-snapshot_ht.ts \
                translations/mx-snapshot_hu.ts \
                translations/mx-snapshot_hy_AM.ts \
                translations/mx-snapshot_hye.ts \
                translations/mx-snapshot_hy.ts \
                translations/mx-snapshot_id.ts \
                translations/mx-snapshot_ie.ts \
                translations/mx-snapshot_is.ts \
                translations/mx-snapshot_it.ts \
                translations/mx-snapshot_ja.ts \
                translations/mx-snapshot_jv.ts \
                translations/mx-snapshot_kab.ts \
                translations/mx-snapshot_ka.ts \
                translations/mx-snapshot_kk.ts \
                translations/mx-snapshot_km.ts \
                translations/mx-snapshot_kn.ts \
                translations/mx-snapshot_ko.ts \
                translations/mx-snapshot_ku.ts \
                translations/mx-snapshot_ky.ts \
                translations/mx-snapshot_lb.ts \
                translations/mx-snapshot_lo.ts \
                translations/mx-snapshot_lt.ts \
                translations/mx-snapshot_lv.ts \
                translations/mx-snapshot_mg.ts \
                translations/mx-snapshot_mi.ts \
                translations/mx-snapshot_mk.ts \
                translations/mx-snapshot_ml.ts \
                translations/mx-snapshot_mn.ts \
                translations/mx-snapshot_mr.ts \
                translations/mx-snapshot_ms.ts \
                translations/mx-snapshot_mt.ts \
                translations/mx-snapshot_my.ts \
                translations/mx-snapshot_nb_NO.ts \
                translations/mx-snapshot_nb.ts \
                translations/mx-snapshot_ne.ts \
                translations/mx-snapshot_nl_BE.ts \
                translations/mx-snapshot_nl.ts \
                translations/mx-snapshot_nn.ts \
                translations/mx-snapshot_ny.ts \
                translations/mx-snapshot_oc.ts \
                translations/mx-snapshot_or.ts \
                translations/mx-snapshot_pa.ts \
                translations/mx-snapshot_pl.ts \
                translations/mx-snapshot_ps.ts \
                translations/mx-snapshot_pt_BR.ts \
                translations/mx-snapshot_pt.ts \
                translations/mx-snapshot_ro.ts \
                translations/mx-snapshot_rue.ts \
                translations/mx-snapshot_ru_RU.ts \
                translations/mx-snapshot_ru.ts \
                translations/mx-snapshot_rw.ts \
                translations/mx-snapshot_sd.ts \
                translations/mx-snapshot_si.ts \
                translations/mx-snapshot_sk.ts \
                translations/mx-snapshot_sl.ts \
                translations/mx-snapshot_sm.ts \
                translations/mx-snapshot_sn.ts \
                translations/mx-snapshot_so.ts \
                translations/mx-snapshot_sq.ts \
                translations/mx-snapshot_sr.ts \
                translations/mx-snapshot_st.ts \
                translations/mx-snapshot_su.ts \
                translations/mx-snapshot_sv.ts \
                translations/mx-snapshot_sw.ts \
                translations/mx-snapshot_ta.ts \
                translations/mx-snapshot_te.ts \
                translations/mx-snapshot_tg.ts \
                translations/mx-snapshot_th.ts \
                translations/mx-snapshot_tk.ts \
                translations/mx-snapshot_tr.ts \
                translations/mx-snapshot_tt.ts \
                translations/mx-snapshot_ug.ts \
                translations/mx-snapshot_uk.ts \
                translations/mx-snapshot_ur.ts \
                translations/mx-snapshot_uz.ts \
                translations/mx-snapshot_vi.ts \
                translations/mx-snapshot_xh.ts \
                translations/mx-snapshot_yi.ts \
                translations/mx-snapshot_yo.ts \
                translations/mx-snapshot_yue_CN.ts \
                translations/mx-snapshot_zh_CN.ts \
                translations/mx-snapshot_zh_HK.ts \
                translations/mx-snapshot_zh_TW.ts
                 
RESOURCES += \
    images.qrc
