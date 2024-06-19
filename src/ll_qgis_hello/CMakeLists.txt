cmake_minimum_required(VERSION 3.5)

project(ll_qgis_hello VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)


add_executable(ll_qgis_hello ${PROJECT_SOURCES})

if(WIN32)
    target_link_libraries(ll_qgis_hello
        Qt${QT_VERSION_MAJOR}::Widgets
        qgis_core.lib
        qgis_gui.lib)
ELSEIF(UNIX)
    target_link_libraries(ll_qgis_hello
        Qt${QT_VERSION_MAJOR}::Widgets
        qgis_core.so
        qgis_gui.so)
ENDIF()
