cmake_minimum_required(VERSION 3.25)
project(VieSchedppGUI LANGUAGES CXX)

# ------------------------------------------------
# C++ standard
# ------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
message(Build: ${CMAKE_BUILD_TYPE})

# ------------------------------------------------
# OpenMP
# ------------------------------------------------
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found!")
endif()

# ------------------------------------------------
# Qt6 modules
# ------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Charts)
message(STATUS "Qt6 found: ${Qt6Core_VERSION}")
message(STATUS "Qt6 modules found: Core, Gui, Widgets, Network, Charts")

# ------------------------------------------------
# Sources, Headers, Forms, Resources
# ------------------------------------------------
set(SRC_FILES
    ../VieSchedpp/Input/LogParser.cpp 
    ../VieSchedpp/Input/SkdCatalogReader.cpp 
    ../VieSchedpp/Input/SkdParser.cpp 
    ../VieSchedpp/Input/StpParser.cpp 
    ../VieSchedpp/Misc/AvoidSatellites.cpp 
    ../VieSchedpp/Misc/AstronomicalParameters.cpp 
    ../VieSchedpp/Misc/AstrometricCalibratorBlock.cpp 
    ../VieSchedpp/Misc/Flags.cpp 
    ../VieSchedpp/Misc/HighImpactScanDescriptor.cpp 
    ../VieSchedpp/Misc/CalibratorBlock.cpp 
    ../VieSchedpp/Misc/ParallacticAngleBlock.cpp 
    ../VieSchedpp/Misc/DifferentialParallacticAngleBlock.cpp 
    ../VieSchedpp/Misc/LookupTable.cpp 
    ../VieSchedpp/Misc/MultiScheduling.cpp 
    ../VieSchedpp/Misc/StationEndposition.cpp 
    ../VieSchedpp/Misc/TimeSystem.cpp 
    ../VieSchedpp/Misc/util.cpp 
    ../VieSchedpp/Misc/VieVS_NamedObject.cpp 
    ../VieSchedpp/Misc/VieVS_Object.cpp 
    ../VieSchedpp/Misc/WeightFactors.cpp 
    ../VieSchedpp/ObservingMode/Bbc.cpp 
    ../VieSchedpp/ObservingMode/Freq.cpp 
    ../VieSchedpp/ObservingMode/If.cpp 
    ../VieSchedpp/ObservingMode/Mode.cpp 
    ../VieSchedpp/ObservingMode/ObservingMode.cpp 
    ../VieSchedpp/ObservingMode/Track.cpp 
    ../VieSchedpp/Output/Output.cpp 
    ../VieSchedpp/Output/Skd.cpp 
    ../VieSchedpp/Output/Vex.cpp 
    ../VieSchedpp/Output/Ast.cpp 
    ../VieSchedpp/Output/SNR_table.cpp 
    ../VieSchedpp/Output/OperationNotes.cpp 
    ../VieSchedpp/Output/SourceStatistics.cpp 
    ../VieSchedpp/Scan/Observation.cpp 
    ../VieSchedpp/Scan/PointingVector.cpp 
    ../VieSchedpp/Scan/Scan.cpp 
    ../VieSchedpp/Scan/ScanTimes.cpp 
    ../VieSchedpp/Scan/Subcon.cpp 
    ../VieSchedpp/Source/Flux/AbstractFlux.cpp 
    ../VieSchedpp/Source/Flux/Flux_B.cpp 
    ../VieSchedpp/Source/Flux/Flux_M.cpp 
    ../VieSchedpp/Source/Flux/Flux_constant.cpp 
    ../VieSchedpp/Source/Flux/Flux_satellite.cpp 
    ../VieSchedpp/Source/AbstractSource.cpp 
    ../VieSchedpp/Source/Quasar.cpp 
    ../VieSchedpp/Source/Satellite.cpp 
    ../VieSchedpp/Source/SourceList.cpp 
    ../VieSchedpp/Station/Antenna/AbstractAntenna.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_GGAO.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_ONSALA_VGOS.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_AzEl.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_AzEl_acceleration.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_HaDc.cpp 
    ../VieSchedpp/Station/Antenna/Antenna_XYew.cpp 
    ../VieSchedpp/Station/CableWrap/AbstractCableWrap.cpp 
    ../VieSchedpp/Station/CableWrap/CableWrap_AzEl.cpp 
    ../VieSchedpp/Station/CableWrap/CableWrap_HaDc.cpp 
    ../VieSchedpp/Station/CableWrap/CableWrap_XYew.cpp 
    ../VieSchedpp/Station/Equip/AbstractEquipment.cpp 
    ../VieSchedpp/Station/Equip/Equipment_elModel.cpp 
    ../VieSchedpp/Station/Equip/Equipment_constant.cpp 
    ../VieSchedpp/Station/Equip/Equipment_elTable.cpp 
    ../VieSchedpp/Station/HorizonMask/AbstractHorizonMask.cpp 
    ../VieSchedpp/Station/HorizonMask/HorizonMask_line.cpp 
    ../VieSchedpp/Station/HorizonMask/HorizonMask_step.cpp 
    ../VieSchedpp/Station/Baseline.cpp 
    ../VieSchedpp/Station/Network.cpp 
    ../VieSchedpp/Station/Position.cpp 
    ../VieSchedpp/Station/SkyCoverage.cpp 
    ../VieSchedpp/Station/Station.cpp 
    ../VieSchedpp/XML/ParameterGroup.cpp 
    ../VieSchedpp/XML/ParameterSettings.cpp 
    ../VieSchedpp/XML/ParameterSetup.cpp 
    ../VieSchedpp/Scheduler.cpp 
    ../VieSchedpp/VieSchedpp.cpp 
    ../VieSchedpp/Initializer.cpp 
    ../VieSchedpp/Algorithm/FocusCorners.cpp 
    ../VieSchedpp/Simulator/Simulator.cpp 
    ../VieSchedpp/Simulator/Solver.cpp 
    ../VieSchedpp/Simulator/Unknown.cpp 
    Utility/downloadmanager.cpp 
    Widgets/calibratorblockwidget.cpp 
    Widgets/mulitschedulingwidget.cpp 
    Widgets/priorities.cpp 
    Widgets/satelliteavoidancewidget.cpp 
    Widgets/setupwidget.cpp 
    Widgets/simulatorwidget.cpp 
    #Widgets/skycoveragewidget.cpp 
    Widgets/sitewidget.cpp 
    Widgets/skycovwidget.cpp 
    Widgets/solverwidget.cpp 
    main.cpp 
    mainwindow.cpp 
    Delegates/comboboxdelegate.cpp 
    Delegates/doublespinboxdelegate.cpp 
    Delegates/spinboxdelegate.cpp 
    Models/model_bbc.cpp 
    Models/model_freq.cpp 
    Models/model_if.cpp 
    Models/model_mode.cpp 
    Models/model_tracks.cpp 
    Parameters/baselineparametersdialog.cpp 
    Parameters/sourceparametersdialog.cpp 
    Parameters/stationparametersdialog.cpp 
    secondaryGUIs/addbanddialog.cpp 
    secondaryGUIs/addgroupdialog.cpp 
    secondaryGUIs/mastersessionviewer.cpp 
    secondaryGUIs/multischededitdialogdatetime.cpp 
    secondaryGUIs/multischededitdialogdouble.cpp 
    secondaryGUIs/multischededitdialogint.cpp 
    secondaryGUIs/obsmodedialog.cpp 
    secondaryGUIs/parsedowntimes.cpp 
    secondaryGUIs/savetosettingsdialog.cpp 
    secondaryGUIs/settingsloadwindow.cpp 
    secondaryGUIs/skedcataloginfo.cpp 
    secondaryGUIs/textfileviewer.cpp 
    secondaryGUIs/tleformat.cpp 
    secondaryGUIs/vieschedpp_analyser.cpp 
    SatelliteGUI/SatelliteForGUI.cpp 
    SatelliteGUI/SatelliteMain.cpp 
    SatelliteGUI/SatelliteObs.cpp 
    SatelliteGUI/SatelliteOutput.cpp 
    SatelliteGUI/satellitescheduling.cpp 
    SatelliteGUI/setTimes.cpp 
    Utility/callout.cpp 
    Utility/chartview.cpp 
    Utility/multicolumnsortfilterproxymodel.cpp 
    Utility/mytextbrowser.cpp 
    Utility/qtutil.cpp 
    Utility/statistics.cpp 
    secondaryGUIs/rendersetup.cpp 
    mainwindows_save_and_load.cpp
)

set(HEADER_FILES
    ../VieSchedpp/Input/LogParser.h 
    ../VieSchedpp/Input/SkdCatalogReader.h 
    ../VieSchedpp/Input/SkdParser.h 
    ../VieSchedpp/Input/StpParser.h 
    ../VieSchedpp/Misc/AvoidSatellites.h 
    ../VieSchedpp/Misc/AstronomicalParameters.h 
    ../VieSchedpp/Misc/AstrometricCalibratorBlock.h 
    ../VieSchedpp/Misc/Constants.h 
    ../VieSchedpp/Misc/Flags.h 
    ../VieSchedpp/Misc/HighImpactScanDescriptor.h 
    ../VieSchedpp/Misc/CalibratorBlock.h 
    ../VieSchedpp/Misc/ParallacticAngleBlock.h 
    ../VieSchedpp/Misc/DifferentialParallacticAngleBlock.h 
    ../VieSchedpp/Misc/LookupTable.h 
    ../VieSchedpp/Misc/MultiScheduling.h 
    ../VieSchedpp/Misc/StationEndposition.h 
    ../VieSchedpp/Misc/Subnetting.h 
    ../VieSchedpp/Misc/TimeSystem.h 
    ../VieSchedpp/Misc/util.h 
    ../VieSchedpp/Misc/VieVS_NamedObject.h 
    ../VieSchedpp/Misc/VieVS_Object.h 
    ../VieSchedpp/Misc/WeightFactors.h 
    ../VieSchedpp/ObservingMode/Bbc.h 
    ../VieSchedpp/ObservingMode/Freq.h 
    ../VieSchedpp/ObservingMode/If.h 
    ../VieSchedpp/ObservingMode/Mode.h 
    ../VieSchedpp/ObservingMode/ObservingMode.h 
    ../VieSchedpp/ObservingMode/Track.h 
    ../VieSchedpp/Output/Output.h 
    ../VieSchedpp/Output/Skd.h 
    ../VieSchedpp/Output/Vex.h 
    ../VieSchedpp/Output/Ast.h 
    ../VieSchedpp/Output/SNR_table.h 
    ../VieSchedpp/Output/OperationNotes.h 
    ../VieSchedpp/Output/SourceStatistics.h 
    ../VieSchedpp/Scan/Observation.h 
    ../VieSchedpp/Scan/PointingVector.h 
    ../VieSchedpp/Scan/Scan.h 
    ../VieSchedpp/Scan/ScanTimes.h 
    ../VieSchedpp/Scan/Subcon.h 
    ../VieSchedpp/Source/Flux/AbstractFlux.h 
    ../VieSchedpp/Source/Flux/Flux_B.h 
    ../VieSchedpp/Source/Flux/Flux_M.h 
    ../VieSchedpp/Source/Flux/Flux_constant.h 
    ../VieSchedpp/Source/Flux/Flux_satellite.h 
    ../VieSchedpp/Source/AbstractSource.h 
    ../VieSchedpp/Source/Quasar.h 
    ../VieSchedpp/Source/Satellite.h 
    ../VieSchedpp/Source/SourceList.h 
    ../VieSchedpp/Station/Antenna/AbstractAntenna.h 
    ../VieSchedpp/Station/Antenna/Antenna_GGAO.h 
    ../VieSchedpp/Station/Antenna/Antenna_ONSALA_VGOS.h 
    ../VieSchedpp/Station/Antenna/Antenna_AzEl.h 
    ../VieSchedpp/Station/Antenna/Antenna_AzEl_acceleration.h 
    ../VieSchedpp/Station/Antenna/Antenna_HaDc.h 
    ../VieSchedpp/Station/Antenna/Antenna_XYew.h 
    ../VieSchedpp/Station/CableWrap/AbstractCableWrap.h 
    ../VieSchedpp/Station/CableWrap/CableWrap_AzEl.h 
    ../VieSchedpp/Station/CableWrap/CableWrap_HaDc.h 
    ../VieSchedpp/Station/CableWrap/CableWrap_XYew.h 
    ../VieSchedpp/Station/Equip/AbstractEquipment.h 
    ../VieSchedpp/Station/Equip/Equipment_elModel.h 
    ../VieSchedpp/Station/Equip/Equipment_constant.h 
    ../VieSchedpp/Station/Equip/Equipment_elTable.h 
    ../VieSchedpp/Station/HorizonMask/AbstractHorizonMask.h 
    ../VieSchedpp/Station/HorizonMask/HorizonMask_line.h 
    ../VieSchedpp/Station/HorizonMask/HorizonMask_step.h 
    ../VieSchedpp/Station/Baseline.h 
    ../VieSchedpp/Station/Network.h 
    ../VieSchedpp/Station/Position.h 
    ../VieSchedpp/Station/SkyCoverage.h 
    ../VieSchedpp/Station/Station.h 
    ../VieSchedpp/XML/ParameterGroup.h 
    ../VieSchedpp/XML/ParameterSettings.h 
    ../VieSchedpp/XML/ParameterSetup.h 
    ../VieSchedpp/Scheduler.h 
    ../VieSchedpp/VieSchedpp.h 
    ../VieSchedpp/Initializer.h 
    ../VieSchedpp/Algorithm/FocusCorners.h 
    ../VieSchedpp/Simulator/Simulator.h 
    ../VieSchedpp/Simulator/Solver.h 
    ../VieSchedpp/Simulator/Unknown.h 
    ../VieSchedpp/SGP4/CoordGeodetic.h 
    ../VieSchedpp/SGP4/Tle.h 
    ../VieSchedpp/SGP4/TleException.h 
    ../VieSchedpp/SGP4/DateTime.h 
    ../VieSchedpp/SGP4/SGP4.h 
    ../VieSchedpp/SGP4/OrbitalElements.h 
    ../VieSchedpp/SGP4/Observer.h 
    ../VieSchedpp/SGP4/Eci.h 
    ../VieSchedpp/Misc/sofa.h
    ../VieSchedpp/Misc/sofam.h
    Delegates/comboboxdelegate.h
    Delegates/doublespinboxdelegate.h 
    Delegates/spinboxdelegate.h 
    Models/model_bbc.h 
    Models/model_freq.h 
    Models/model_if.h 
    Models/model_mode.h 
    Models/model_tracks.h 
    Parameters/baselineparametersdialog.h 
    Parameters/sourceparametersdialog.h 
    Parameters/stationparametersdialog.h 
    Utility/downloadmanager.h 
    Widgets/calibratorblockwidget.h 
    Widgets/mulitschedulingwidget.h 
    Widgets/priorities.h 
    Widgets/satelliteavoidancewidget.h 
    Widgets/setupwidget.h 
    Widgets/simulatorwidget.h 
    #Widgets/skycoveragewidget.h 
    Widgets/sitewidget.h 
    Widgets/skycovwidget.h 
    Widgets/solverwidget.h 
    secondaryGUIs/addbanddialog.h 
    secondaryGUIs/addgroupdialog.h 
    secondaryGUIs/mastersessionviewer.h 
    secondaryGUIs/multischededitdialogdatetime.h 
    secondaryGUIs/multischededitdialogdouble.h 
    secondaryGUIs/multischededitdialogint.h 
    secondaryGUIs/obsmodedialog.h 
    secondaryGUIs/parsedowntimes.h 
    secondaryGUIs/savetosettingsdialog.h 
    secondaryGUIs/settingsloadwindow.h 
    secondaryGUIs/skedcataloginfo.h 
    secondaryGUIs/textfileviewer.h 
    secondaryGUIs/tleformat.h 
    secondaryGUIs/vieschedpp_analyser.h 
    SatelliteGUI/SatelliteForGUI.h 
    SatelliteGUI/SatelliteMain.h 
    SatelliteGUI/SatelliteObs.h 
    SatelliteGUI/SatelliteOutput.h 
    SatelliteGUI/satellitescheduling.h 
    SatelliteGUI/setTimes.h 
    Utility/callout.h 
    Utility/chartview.h 
    Utility/multicolumnsortfilterproxymodel.h 
    Utility/mytextbrowser.h 
    Utility/qtutil.h 
    mainwindow.h 
    Utility/statistics.h 
    secondaryGUIs/rendersetup.h
)

# List all .ui files
set(UI_FILES
    Widgets/calibratorblockwidget.ui
    Widgets/mulitschedulingwidget.ui
    Widgets/priorities.ui
    Widgets/satelliteavoidancewidget.ui
    Widgets/setupwidget.ui
    Widgets/simulatorwidget.ui
    Widgets/sitewidget.ui
    Widgets/skycovwidget.ui
    Widgets/solverwidget.ui
    mainwindow.ui
    Parameters/baselineparametersdialog.ui
    Parameters/sourceparametersdialog.ui
    Parameters/stationparametersdialog.ui
    secondaryGUIs/addbanddialog.ui
    secondaryGUIs/addgroupdialog.ui
    secondaryGUIs/mastersessionviewer.ui
    secondaryGUIs/multischededitdialogdatetime.ui
    secondaryGUIs/multischededitdialogdouble.ui
    secondaryGUIs/multischededitdialogint.ui
    secondaryGUIs/obsmodedialog.ui
    secondaryGUIs/parsedowntimes.ui
    secondaryGUIs/savetosettingsdialog.ui
    secondaryGUIs/settingsloadwindow.ui
    secondaryGUIs/skedcataloginfo.ui
    secondaryGUIs/textfileviewer.ui
    secondaryGUIs/tleformat.ui
    secondaryGUIs/vieschedpp_analyser.ui
    secondaryGUIs/rendersetup.ui
    SatelliteGUI/satellitescheduling.ui
    SatelliteGUI/setTimes.ui
)

set(QRC_FILES
    myresources.qrc
)
qt_add_resources(RESOURCES_RCC ${QRC_FILES})

# ------------------------------------------------
# Tell CMake/Qt6 to generate the ui_*.h files automatically
# ------------------------------------------------
qt6_wrap_ui(GENERATED_UI_HEADERS ${UI_FILES})

# ------------------------------------------------
# Define target
# ------------------------------------------------
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
    ${GENERATED_UI_HEADERS}
    ${RESOURCES_RCC}
)

# ------------------------------------------------
# Include directories
# ------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ../VieSchedpp/EIGEN
    ../VieSchedpp/EIGEN/Dense
    .
    Widgets
    secondaryGUIs
    Utility
    Delegates
    Models
    Parameters
    SatelliteGUI
     ${CMAKE_CURRENT_BINARY_DIR}
)

# ------------------------------------------------
# Compile definitions
# ------------------------------------------------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    VieSchedppOnline=1
    QT_DEPRECATED_WARNINGS
    BOOST_ALL_NO_LIB
)

# ------------------------------------------------
# Qt integration
# ------------------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${RESOURCES_RCC})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

# ------------------------------------------------
# External libraries (IAU_SOFA and SGP4)
# ------------------------------------------------
if(UNIX)
    # IAU SOFA library
    if(NOT DEFINED ENV{IAU_SOFA})
        set(SOFA_PATH "${CMAKE_SOURCE_DIR}/../IAU_SOFA/Release/libsofa_c.a")
        if(EXISTS "${SOFA_PATH}")
            target_link_libraries(${PROJECT_NAME} PRIVATE "${SOFA_PATH}")
            message(STATUS "IAU SOFA found at ${SOFA_PATH}")
        else()
            message(WARNING "IAU SOFA not found at ${SOFA_PATH}")
        endif()
    else()
        set(SOFA_PATH "$ENV{IAU_SOFA}")
        if(EXISTS "${SOFA_PATH}")
            target_link_libraries(${PROJECT_NAME} PRIVATE "${SOFA_PATH}")
            message(STATUS "IAU SOFA found at ${SOFA_PATH}")
        else()
            message(WARNING "IAU SOFA not found at ${SOFA_PATH}")
        endif()
    endif()

    # SGP4 library
    if(NOT DEFINED ENV{SGP4})
        set(SGP4_PATH "${CMAKE_SOURCE_DIR}/../sgp4/Release/libsgp4/libsgp4.a")
        if(EXISTS "${SGP4_PATH}")
            target_link_libraries(${PROJECT_NAME} PRIVATE "${SGP4_PATH}")
            message(STATUS "SGP4 library found at ${SGP4_PATH}")
        else()
            message(WARNING "SGP4 library not found at ${SGP4_PATH}")
        endif()
    else()
        set(SGP4_PATH "$ENV{SGP4}")
        if(EXISTS "${SGP4_PATH}")
            target_link_libraries(${PROJECT_NAME} PRIVATE "${SGP4_PATH}")
            message(STATUS "SGP4 library found at ${SGP4_PATH}")
        else()
            message(WARNING "SGP4 library not found at ${SGP4_PATH}")
        endif()
    endif()
endif()

# ------------------------------------------------
# Git commit hashes
# ------------------------------------------------
execute_process(
    COMMAND git log -1 --date=format:%Y-%m-%d --format=v.%cd
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(GIT_COMMIT_HASH)
    message(STATUS "VieSched++ GUI Git commit: ${GIT_COMMIT_HASH}")
else()
    message(WARNING "VieSched++ GUI Git commit not found")
endif()

execute_process(
    COMMAND git -C ../VieSchedpp log -1 --date=format:%Y-%m-%d --format=v.%cd
    OUTPUT_VARIABLE GIT_SCHEDULER_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(GIT_SCHEDULER_COMMIT_HASH)
    message(STATUS "VieSched++ Scheduler Git commit: ${GIT_SCHEDULER_COMMIT_HASH}")
else()
    message(WARNING "VieSched++ Scheduler Git commit not found")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GIT_COMMIT_HASH="${GIT_COMMIT_HASH}"
    GIT_SCHEDULER_COMMIT_HASH="${GIT_SCHEDULER_COMMIT_HASH}"
)
