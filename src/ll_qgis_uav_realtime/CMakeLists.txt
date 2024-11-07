cmake_minimum_required(VERSION 3.5)

project(ll_qgis_uav_realtime VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)
include_directories(${PROJECT_SOURCE_DIR}/../ll_qgis_base_lib/) # 指定头文件搜索路径


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Svg REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        paramdockwidget.h paramdockwidget.cpp paramdockwidget.ui
)

add_executable(ll_qgis_uav_realtime ${PROJECT_SOURCES})

if(WIN32)
    target_link_libraries(ll_qgis_uav_realtime
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Svg
        ll_qgis_base_lib
        qgis_app.lib
        qgis_core.lib
        qgis_gui.lib
        qgis_analysis.lib)
ELSEIF(UNIX)
    target_link_libraries(ll_qgis_uav_realtime
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Svg
        ll_qgis_base_lib.so
        qgis_app.so
        qgis_core.so
        qgis_gui.so
        qgis_analysis.so)
ENDIF()
