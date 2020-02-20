 # 
 #  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 #  Copyright (C) 2018  Matthias Schartner
 #
 #  This program is free software: you can redistribute it and/or modify
 #  it under the terms of the GNU General Public License as published by
 #  the Free Software Foundation, either version 3 of the License, or
 #  (at your option) any later version.
 #
 #  This program is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.
 #  
 #  You should have received a copy of the GNU General Public License
 #  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 #

#-------------------------------------------------
#
# Project created by QtCreator 2017-09-29T11:36:13
#
#-------------------------------------------------
CONFIG += c++14

QMAKE_CXXFLAGS+= -fopenmp
LIBS += -fopenmp

QT += core gui charts
DEFINES += VieSchedppOnline=false

# Comment the following lines for offline installation only without QT NETWORK
# -----------------------
QT += network
DEFINES += VieSchedppOnline=true
# -----------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VieSchedppGUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += /home/mschartn/boost_1_64_0/
#LIBS += "-LC:/home/mschartn/boost_1_64_0//stage/lib/"
INCLUDEPATH += C:/MinGW/include
INCLUDEPATH += ../VieSchedpp/
LIBS += ../IAU_SOFA/Release/libsofa_c.a

SOURCES += \
    Utility/downloadmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    Delegates/comboboxdelegate.cpp \
    Delegates/doublespinboxdelegate.cpp \
    Delegates/spinboxdelegate.cpp \
    Models/model_bbc.cpp \
    Models/model_freq.cpp \
    Models/model_if.cpp \
    Models/model_mode.cpp \
    Models/model_tracks.cpp \
    Parameters/baselineparametersdialog.cpp \
    Parameters/sourceparametersdialog.cpp \
    Parameters/stationparametersdialog.cpp \
    secondaryGUIs/addbanddialog.cpp \
    secondaryGUIs/addgroupdialog.cpp \
    secondaryGUIs/mastersessionviewer.cpp \
    secondaryGUIs/multischededitdialogdatetime.cpp \
    secondaryGUIs/multischededitdialogdouble.cpp \
    secondaryGUIs/multischededitdialogint.cpp \
    secondaryGUIs/obsmodedialog.cpp \
    secondaryGUIs/parsedowntimes.cpp \
    secondaryGUIs/savetosettingsdialog.cpp \
    secondaryGUIs/settingsloadwindow.cpp \
    Widgets/simulator.cpp \
    secondaryGUIs/skedcataloginfo.cpp \
    secondaryGUIs/textfileviewer.cpp \
    secondaryGUIs/vieschedpp_analyser.cpp \
    Utility/callout.cpp \
    Utility/chartview.cpp \
    Utility/multicolumnsortfilterproxymodel.cpp \
    Utility/mytextbrowser.cpp \
    Utility/qtutil.cpp \
    Utility/statistics.cpp \
    secondaryGUIs/rendersetup.cpp \
    mainwindows_save_and_load.cpp

HEADERS += \
    Delegates/comboboxdelegate.h \
    Delegates/doublespinboxdelegate.h \
    Delegates/spinboxdelegate.h \
    Models/model_bbc.h \
    Models/model_freq.h \
    Models/model_if.h \
    Models/model_mode.h \
    Models/model_tracks.h \
    Parameters/baselineparametersdialog.h \
    Parameters/sourceparametersdialog.h \
    Parameters/stationparametersdialog.h \
    Utility/downloadmanager.h \
    secondaryGUIs/addbanddialog.h \
    secondaryGUIs/addgroupdialog.h \
    secondaryGUIs/mastersessionviewer.h \
    secondaryGUIs/multischededitdialogdatetime.h \
    secondaryGUIs/multischededitdialogdouble.h \
    secondaryGUIs/multischededitdialogint.h \
    secondaryGUIs/obsmodedialog.h \
    secondaryGUIs/parsedowntimes.h \
    secondaryGUIs/savetosettingsdialog.h \
    secondaryGUIs/settingsloadwindow.h \
    Widgets/simulator.h \
    secondaryGUIs/skedcataloginfo.h \
    secondaryGUIs/textfileviewer.h \
    secondaryGUIs/vieschedpp_analyser.h \
    Utility/callout.h \
    Utility/chartview.h \
    Utility/multicolumnsortfilterproxymodel.h \
    Utility/mytextbrowser.h \
    Utility/qtutil.h \
    mainwindow.h \
    Utility/statistics.h \
    secondaryGUIs/rendersetup.h

FORMS += \
    mainwindow.ui \
    Parameters/baselineparametersdialog.ui \
    Parameters/sourceparametersdialog.ui \
    Parameters/stationparametersdialog.ui \
    secondaryGUIs/addbanddialog.ui \
    secondaryGUIs/addgroupdialog.ui \
    secondaryGUIs/mastersessionviewer.ui \
    secondaryGUIs/multischededitdialogdatetime.ui \
    secondaryGUIs/multischededitdialogdouble.ui \
    secondaryGUIs/multischededitdialogint.ui \
    secondaryGUIs/obsmodedialog.ui \
    secondaryGUIs/parsedowntimes.ui \
    secondaryGUIs/savetosettingsdialog.ui \
    secondaryGUIs/settingsloadwindow.ui \
    Widgets/simulator.ui \
    secondaryGUIs/skedcataloginfo.ui \
    secondaryGUIs/textfileviewer.ui \
    secondaryGUIs/vieschedpp_analyser.ui \
    secondaryGUIs/rendersetup.ui

RESOURCES += \
        myresources.qrc

DISTFILES += \
        VLBI_Scheduler/libsofa_c.a \
        VLBI_Scheduler/CMakeLists.txt

#exists( $$PWD/.git) {
exists( .git) {
    GIT_COMMIT_HASH = $$system(git log -1 --format=%H)
}else{
    GIT_COMMIT_HASH = "unknown"
}

exists( ../VieSchedpp/.git){
    GIT_SCHEDULER_COMMIT_HASH = $$system(cd ../VieSchedpp; git log -1 --format=%H)
}else{
    GIT_SCHEDULER_COMMIT_HASH = "unknown"
}

message(VieSched++ GUI version $$GIT_COMMIT_HASH)
message(VieSched++ version $$GIT_SCHEDULER_COMMIT_HASH)

DEFINES += GIT_COMMIT_HASH=\\\"$$GIT_COMMIT_HASH\\\"
DEFINES += GIT_SCHEDULER_COMMIT_HASH=\\\"$$GIT_SCHEDULER_COMMIT_HASH\\\"
