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
CONFIG(release, debug|release):message(Release build)
CONFIG(debug, debug|release):message(Debug build)

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

INCLUDEPATH += ../VieSchedpp/EIGEN
INCLUDEPATH += ../VieSchedpp/EIGEN/Dense
unix {
    IAU_SOFA_PATH=$${IAU_SOFA}
    isEmpty(IAU_SOFA_PATH) {
        LIBS += ../IAU_SOFA/Release/libsofa_c.a
    } else {
        LIBS += $${IAU_SOFA_PATH}
    }
}

# for my windows builds
win32{
    BOOST_PATH=$${BOOST}
    isEmpty(BOOST_PATH) {
        INCLUDEPATH += ../boost_1_72_0
    } else {
        INCLUDEPATH += $${BOOST_PATH}
    }

    IAU_SOFA_PATH=$${IAU_SOFA}
    isEmpty(IAU_SOFA_PATH) {
        LIBS += ../IAU_SOFA/Release/sofa_c.lib
    } else {
        LIBS += $${IAU_SOFA_PATH}
    }
}

SOURCES += \
    ../VieSchedpp/Input/LogParser.cpp \
    ../VieSchedpp/Input/SkdCatalogReader.cpp \
    ../VieSchedpp/Input/SkdParser.cpp \
    ../VieSchedpp/Misc/AstronomicalParameters.cpp \
    ../VieSchedpp/Misc/AstrometricCalibratorBlock.cpp \
    ../VieSchedpp/Misc/Flags.cpp \
    ../VieSchedpp/Misc/HighImpactScanDescriptor.cpp \
    ../VieSchedpp/Misc/CalibratorBlock.cpp \
    ../VieSchedpp/Misc/LookupTable.cpp \
    ../VieSchedpp/Misc/MultiScheduling.cpp \
    ../VieSchedpp/Misc/StationEndposition.cpp \
    ../VieSchedpp/Misc/TimeSystem.cpp \
    ../VieSchedpp/Misc/util.cpp \
    ../VieSchedpp/Misc/VieVS_NamedObject.cpp \
    ../VieSchedpp/Misc/VieVS_Object.cpp \
    ../VieSchedpp/Misc/WeightFactors.cpp \
    ../VieSchedpp/ObservingMode/Bbc.cpp \
    ../VieSchedpp/ObservingMode/Freq.cpp \
    ../VieSchedpp/ObservingMode/If.cpp \
    ../VieSchedpp/ObservingMode/Mode.cpp \
    ../VieSchedpp/ObservingMode/ObservingMode.cpp \
    ../VieSchedpp/ObservingMode/Track.cpp \
    ../VieSchedpp/Output/Output.cpp \
    ../VieSchedpp/Output/Skd.cpp \
    ../VieSchedpp/Output/Vex.cpp \
    ../VieSchedpp/Output/Ast.cpp \
    ../VieSchedpp/Output/SNR_table.cpp \
    ../VieSchedpp/Output/OperationNotes.cpp \
    ../VieSchedpp/Output/SourceStatistics.cpp \
    ../VieSchedpp/Scan/Observation.cpp \
    ../VieSchedpp/Scan/PointingVector.cpp \
    ../VieSchedpp/Scan/Scan.cpp \
    ../VieSchedpp/Scan/ScanTimes.cpp \
    ../VieSchedpp/Scan/Subcon.cpp \
    ../VieSchedpp/Source/Flux/AbstractFlux.cpp \
    ../VieSchedpp/Source/Flux/Flux_B.cpp \
    ../VieSchedpp/Source/Flux/Flux_M.cpp \
    ../VieSchedpp/Source/AbstractSource.cpp \
    ../VieSchedpp/Source/Quasar.cpp \
    ../VieSchedpp/Source/Satellite.cpp \
    ../VieSchedpp/Source/SourceList.cpp \
    ../VieSchedpp/Station/Antenna/AbstractAntenna.cpp \
    ../VieSchedpp/Station/Antenna/Antenna_AzEl.cpp \
    ../VieSchedpp/Station/Antenna/Antenna_HaDc.cpp \
    ../VieSchedpp/Station/Antenna/Antenna_XYew.cpp \
    ../VieSchedpp/Station/CableWrap/AbstractCableWrap.cpp \
    ../VieSchedpp/Station/CableWrap/CableWrap_AzEl.cpp \
    ../VieSchedpp/Station/CableWrap/CableWrap_HaDc.cpp \
    ../VieSchedpp/Station/CableWrap/CableWrap_XYew.cpp \
    ../VieSchedpp/Station/Equip/Equipment.cpp \
    ../VieSchedpp/Station/Equip/Equipment_elDependent.cpp \
    ../VieSchedpp/Station/HorizonMask/AbstractHorizonMask.cpp \
    ../VieSchedpp/Station/HorizonMask/HorizonMask_line.cpp \
    ../VieSchedpp/Station/HorizonMask/HorizonMask_step.cpp \
    ../VieSchedpp/Station/Baseline.cpp \
    ../VieSchedpp/Station/Network.cpp \
    ../VieSchedpp/Station/Position.cpp \
    ../VieSchedpp/Station/SkyCoverage.cpp \
    ../VieSchedpp/Station/Station.cpp \
    ../VieSchedpp/XML/ParameterGroup.cpp \
    ../VieSchedpp/XML/ParameterSettings.cpp \
    ../VieSchedpp/XML/ParameterSetup.cpp \
    ../VieSchedpp/Scheduler.cpp \
    ../VieSchedpp/VieSchedpp.cpp \
    ../VieSchedpp/Initializer.cpp \
    ../VieSchedpp/Algorithm/FocusCorners.cpp \
    ../VieSchedpp/Simulator/Simulator.cpp \
    ../VieSchedpp/Simulator/Solver.cpp \
    ../VieSchedpp/Simulator/Unknown.cpp \
    Utility/downloadmanager.cpp \
    Widgets/mulitschedulingwidget.cpp \
    Widgets/priorities.cpp \
    Widgets/simulatorwidget.cpp \
    Widgets/solverwidget.cpp \
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
    ../VieSchedpp/Input/LogParser.h \
    ../VieSchedpp/Input/SkdCatalogReader.h \
    ../VieSchedpp/Input/SkdParser.h \
    ../VieSchedpp/Misc/AstronomicalParameters.h \
    ../VieSchedpp/Misc/AstrometricCalibratorBlock.h \
    ../VieSchedpp/Misc/Constants.h \
    ../VieSchedpp/Misc/Flags.h \
    ../VieSchedpp/Misc/HighImpactScanDescriptor.h \
    ../VieSchedpp/Misc/CalibratorBlock.h \
    ../VieSchedpp/Misc/LookupTable.h \
    ../VieSchedpp/Misc/MultiScheduling.h \
    ../VieSchedpp/Misc/sofa.h \
    ../VieSchedpp/Misc/sofam.h \
    ../VieSchedpp/Misc/StationEndposition.h \
    ../VieSchedpp/Misc/Subnetting.h \
    ../VieSchedpp/Misc/TimeSystem.h \
    ../VieSchedpp/Misc/util.h \
    ../VieSchedpp/Misc/VieVS_NamedObject.h \
    ../VieSchedpp/Misc/VieVS_Object.h \
    ../VieSchedpp/Misc/WeightFactors.h \
    ../VieSchedpp/ObservingMode/Bbc.h \
    ../VieSchedpp/ObservingMode/Freq.h \
    ../VieSchedpp/ObservingMode/If.h \
    ../VieSchedpp/ObservingMode/Mode.h \
    ../VieSchedpp/ObservingMode/ObservingMode.h \
    ../VieSchedpp/ObservingMode/Track.h \
    ../VieSchedpp/Output/Output.h \
    ../VieSchedpp/Output/Skd.h \
    ../VieSchedpp/Output/Vex.h \
    ../VieSchedpp/Output/Ast.h \
    ../VieSchedpp/Output/SNR_table.h \
    ../VieSchedpp/Output/OperationNotes.h \
    ../VieSchedpp/Output/SourceStatistics.h \
    ../VieSchedpp/Scan/Observation.h \
    ../VieSchedpp/Scan/PointingVector.h \
    ../VieSchedpp/Scan/Scan.h \
    ../VieSchedpp/Scan/ScanTimes.h \
    ../VieSchedpp/Scan/Subcon.h \
    ../VieSchedpp/Source/Flux/AbstractFlux.h \
    ../VieSchedpp/Source/Flux/Flux_B.h \
    ../VieSchedpp/Source/Flux/Flux_M.h \
    ../VieSchedpp/Source/AbstractSource.h \
    ../VieSchedpp/Source/Quasar.h \
    ../VieSchedpp/Source/Satellite.h \
    ../VieSchedpp/Source/SourceList.h \
    ../VieSchedpp/Station/Antenna/AbstractAntenna.h \
    ../VieSchedpp/Station/Antenna/Antenna_AzEl.h \
    ../VieSchedpp/Station/Antenna/Antenna_HaDc.h \
    ../VieSchedpp/Station/Antenna/Antenna_XYew.h \
    ../VieSchedpp/Station/CableWrap/AbstractCableWrap.h \
    ../VieSchedpp/Station/CableWrap/CableWrap_AzEl.h \
    ../VieSchedpp/Station/CableWrap/CableWrap_HaDc.h \
    ../VieSchedpp/Station/CableWrap/CableWrap_XYew.h \
    ../VieSchedpp/Station/Equip/Equipment.h \
    ../VieSchedpp/Station/Equip/Equipment_elDependent.h \
    ../VieSchedpp/Station/HorizonMask/AbstractHorizonMask.h \
    ../VieSchedpp/Station/HorizonMask/HorizonMask_line.h \
    ../VieSchedpp/Station/HorizonMask/HorizonMask_step.h \
    ../VieSchedpp/Station/Baseline.h \
    ../VieSchedpp/Station/Network.h \
    ../VieSchedpp/Station/Position.h \
    ../VieSchedpp/Station/SkyCoverage.h \
    ../VieSchedpp/Station/Station.h \
    ../VieSchedpp/XML/ParameterGroup.h \
    ../VieSchedpp/XML/ParameterSettings.h \
    ../VieSchedpp/XML/ParameterSetup.h \
    ../VieSchedpp/Scheduler.h \
    ../VieSchedpp/VieSchedpp.h \
    ../VieSchedpp/Initializer.h \
    ../VieSchedpp/Algorithm/FocusCorners.h \
    ../VieSchedpp/Simulator/Simulator.h \
    ../VieSchedpp/Simulator/Solver.h \
    ../VieSchedpp/Simulator/Unknown.h \
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
    Widgets/mulitschedulingwidget.h \
    Widgets/priorities.h \
    Widgets/simulatorwidget.h \
    Widgets/solverwidget.h \
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
    Widgets/mulitschedulingwidget.ui \
    Widgets/priorities.ui \
    Widgets/simulatorwidget.ui \
    Widgets/solverwidget.ui \
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
DEFINES += BOOST_ALL_NO_LIB

message(VieSched++ GUI version $$GIT_COMMIT_HASH)
message(VieSched++ version $$GIT_SCHEDULER_COMMIT_HASH)

DEFINES += GIT_COMMIT_HASH=\\\"$$GIT_COMMIT_HASH\\\"
DEFINES += GIT_SCHEDULER_COMMIT_HASH=\\\"$$GIT_SCHEDULER_COMMIT_HASH\\\"
