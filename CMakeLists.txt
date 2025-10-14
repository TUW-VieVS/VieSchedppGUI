cmake_minimum_required(VERSION 3.25)
project(VieSchedppGUI LANGUAGES CXX)

# ------------------------------------------------
# C++ standard
# ------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------
# OpenMP
# ------------------------------------------------
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found!")
endif()

# ------------------------------------------------
# Qt6 modules
# ------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Charts)
message(STATUS "Qt6 found: ${Qt6Core_VERSION}")
message(STATUS "Qt6 modules found: Core, Gui, Widgets, Network, Charts")

# ------------------------------------------------
# Sources, Headers, Forms, Resources
# ------------------------------------------------
# VieSched++ sources
file(GLOB VIESCHEDPP_ROOT_SRC "../VieSchedpp/*.cpp" "../VieSchedpp/*.h")
list(FILTER VIESCHEDPP_ROOT_SRC EXCLUDE REGEX ".*/VieSchedpp/main\\.cpp$")
file(GLOB_RECURSE VIESCHEDPP_ALGORITHM_SRC    "../VieSchedpp/Algorithm/*.cpp"    "../VieSchedpp/Algorithm/*.h")
file(GLOB_RECURSE VIESCHEDPP_STATION_SRC      "../VieSchedpp/Station/*.cpp"      "../VieSchedpp/Station/*.h")
file(GLOB_RECURSE VIESCHEDPP_SOURCE_SRC       "../VieSchedpp/Source/*.cpp"       "../VieSchedpp/Source/*.h")
file(GLOB_RECURSE VIESCHEDPP_SCAN_SRC         "../VieSchedpp/Scan/*.cpp"         "../VieSchedpp/Scan/*.h")
file(GLOB_RECURSE VIESCHEDPP_MISC_SRC         "../VieSchedpp/Misc/*.cpp"         "../VieSchedpp/Misc/*.h")
file(GLOB_RECURSE VIESCHEDPP_INPUT_SRC        "../VieSchedpp/Input/*.cpp"        "../VieSchedpp/Input/*.h")
file(GLOB_RECURSE VIESCHEDPP_OUTPUT_SRC       "../VieSchedpp/Output/*.cpp"       "../VieSchedpp/Output/*.h")
file(GLOB_RECURSE VIESCHEDPP_OBS_MODE_SRC     "../VieSchedpp/ObservingMode/*.cpp" "../VieSchedpp/ObservingMode/*.h")
file(GLOB_RECURSE VIESCHEDPP_SIM_SRC          "../VieSchedpp/Simulator/*.cpp"    "../VieSchedpp/Simulator/*.h")
file(GLOB_RECURSE VIESCHEDPP_XML_SRC          "../VieSchedpp/XML/*.cpp"          "../VieSchedpp/XML/*.h")
file(GLOB_RECURSE VIESCHEDPP_SGP4_SRC         "../VieSchedpp/SGP4/*.h")  # SGP4 is header-only

# GUI sources
file(GLOB GUI_SRC *.cpp *.h)
# Widgets, Delegates, Models, Parameters, secondaryGUIs, SatelliteGUI, Utility
file(GLOB_RECURSE WIDGETS_SRC Widgets/*.cpp Widgets/*.h)
file(GLOB_RECURSE DELEGATES_SRC Delegates/*.cpp Delegates/*.h)
file(GLOB_RECURSE MODELS_SRC Models/*.cpp Models/*.h)
file(GLOB_RECURSE PARAMETERS_SRC Parameters/*.cpp Parameters/*.h)
file(GLOB_RECURSE SECONDARY_SRC secondaryGUIs/*.cpp secondaryGUIs/*.h)
file(GLOB_RECURSE SATELLITE_SRC SatelliteGUI/*.cpp SatelliteGUI/*.h)
file(GLOB_RECURSE UTILITY_SRC Utility/*.cpp Utility/*.h)

# Combine all sources
set(ALL_SOURCES
    ${VIESCHEDPP_ROOT_SRC}
    ${VIESCHEDPP_ALGORITHM_SRC}
    ${VIESCHEDPP_STATION_SRC}
    ${VIESCHEDPP_SOURCE_SRC}
    ${VIESCHEDPP_SCAN_SRC}
    ${VIESCHEDPP_MISC_SRC}
    ${VIESCHEDPP_INPUT_SRC}
    ${VIESCHEDPP_OUTPUT_SRC}
    ${VIESCHEDPP_OBS_MODE_SRC}
    ${VIESCHEDPP_SIM_SRC}
    ${VIESCHEDPP_XML_SRC}
    ${VIESCHEDPP_SGP4_SRC}
    ${GUI_SRC}
    ${WIDGETS_SRC}
    ${DELEGATES_SRC}
    ${MODELS_SRC}
    ${PARAMETERS_SRC}
    ${SECONDARY_SRC}
    ${SATELLITE_SRC}
    ${UTILITY_SRC}
)

# List all .ui files
file(GLOB_RECURSE UI_FILES
    Widgets/*.ui
    Parameters/*.ui
    secondaryGUIs/*.ui
    SatelliteGUI/*.ui
    *.ui
)

file(GLOB_RECURSE QRC_FILES *.qrc)
qt_add_resources(RESOURCES_RCC ${QRC_FILES})

# ------------------------------------------------
# Tell CMake/Qt6 to generate the ui_*.h files automatically
# ------------------------------------------------
# qt6_wrap_ui(GENERATED_UI_HEADERS ${UI_FILES})

# ------------------------------------------------
# Add executable
# ------------------------------------------------
add_executable(${PROJECT_NAME}
    ${ALL_SOURCES}
    ${UI_FILES}
    ${RESOURCES_RCC}
)

# ------------------------------------------------
# Include directories
# ------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
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
set(BUILD_SIMULATIONS OFF)
message(STATUS "Include VieSched++ without simulator integration")

# ------------------------------------------------
# Link Qt6 libraries
# ------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

# ------------------------------------------------
# BOOST
# ------------------------------------------------
option(LINK_BOOST "Link against Boost libraries" ON)
set(PATH_BOOST_ROOT "" CACHE PATH "Optional path to Boost installation")
if(LINK_BOOST)
    message(STATUS "Linking against Boost libraries")
    # If user provided a custom Boost path, set the relevant variables
    if (PATH_BOOST_ROOT)
        set(Boost_ROOT "${PATH_BOOST_ROOT}")
        set(BOOST_INCLUDEDIR "${PATH_BOOST_ROOT}/include")
        set(BOOST_LIBRARYDIR "${PATH_BOOST_ROOT}/lib")
        message(STATUS "Using Boost from custom path: ${PATH_BOOST_ROOT}")
    endif ()
    find_package(Boost CONFIG REQUIRED COMPONENTS log log_setup system)
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log Boost::log_setup Boost::system)
else()
    message(STATUS "Using Boost header-only mode")
    find_package(Boost CONFIG REQUIRED)
endif()

if (Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    if (Boost_LIBRARIES)
        message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
    endif ()
else ()
    message(FATAL_ERROR "Boost not found!")
endif ()

# ------------------------------------------------------------------------------
# SOFA
# ------------------------------------------------------------------------------
set(PATH_IAU_SOFA "../IAU_SOFA/Release/" CACHE PATH "Path to IAU SOFA library")
find_library(SOFA_LIB
        NAMES sofa_c libsofa_c.a libsofa_c.lib
        HINTS ${PATH_IAU_SOFA}
        NO_DEFAULT_PATH
)

if(SOFA_LIB)
    message(STATUS "Found SOFA library: ${SOFA_LIB}")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SOFA_LIB})
else()
    message(WARNING "SOFA library not found! Please set PATH_IAU_SOFA.")
endif()

# ------------------------------------------------------------------------------
# SGP4
# ------------------------------------------------------------------------------
set(PATH_SGP4 "../sgp4/Release/libsgp4/" CACHE PATH "Path to SGP4 library")
find_library(SGP4_LIB
        NAMES sgp4 libsgp4.a libsgp4.lib
        HINTS ${PATH_SGP4}
        NO_DEFAULT_PATH
)

if(SGP4_LIB)
    message(STATUS "Found SGP4 library: ${SGP4_LIB}")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SGP4_LIB})
else()
    message(WARNING "SGP4 library not found! Please set PATH_SGP4.")
endif()

# ------------------------------------------------------------------------------
# Generate Git version header for both GUI and Scheduler
# ------------------------------------------------------------------------------

set(GIT_VERSION_HEADER "${CMAKE_BINARY_DIR}/generated/version.h")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")

# Get GUI version
execute_process(
    COMMAND git -C "${CMAKE_CURRENT_SOURCE_DIR}" log -1 --date=format:%Y-%m-%d --format=v.%cd
    OUTPUT_VARIABLE VIESCHEDPP_GUI_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT VIESCHEDPP_GUI_VERSION)
    set(VIESCHEDPP_GUI_VERSION "v.unknown")
    message(WARNING "GIT not found -> no version could be extracted: GUI ${VIESCHEDPP_GUI_VERSION}")
else()
    message(STATUS "VieSched++ GUI version: ${VIESCHEDPP_GUI_VERSION}")
endif()

# Get Scheduler version (relative path to the scheduler project)
execute_process(
    COMMAND git -C "${CMAKE_CURRENT_SOURCE_DIR}/../VieSchedpp" log -1 --date=format:%Y-%m-%d --format=v.%cd
    OUTPUT_VARIABLE VIESCHEDPP_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT VIESCHEDPP_VERSION)
    set(VIESCHEDPP_VERSION "v.unknown")
    message(WARNING "GIT not found -> no version could be extracted: Scheduler ${VIESCHEDPP_VERSION}")
else()
    message(STATUS "VieSched++ Scheduler version: ${VIESCHEDPP_VERSION}")
endif()

# Create a custom command to write the header
add_custom_command(
    OUTPUT "${GIT_VERSION_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "/* Auto-generated Git version header */" > "${GIT_VERSION_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" >> "${GIT_VERSION_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#define VIESCHEDPP_GUI_VERSION \"${VIESCHEDPP_GUI_VERSION}\"" >> "${GIT_VERSION_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E echo "#define VIESCHEDPP_VERSION \"${VIESCHEDPP_VERSION}\"" >> "${GIT_VERSION_HEADER}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.git/HEAD" "../VieSchedpp/.git/HEAD"
    COMMENT "Generating Git version header: GUI=${VIESCHEDPP_GUI_VERSION}, Scheduler=${VIESCHEDPP_VERSION}"
    VERBATIM
)

# Target to generate header
add_custom_target(generate_git_version ALL
    DEPENDS "${GIT_VERSION_HEADER}"
)

# Make the main project depend on the generated file
add_dependencies(${PROJECT_NAME} generate_git_version)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/generated")
