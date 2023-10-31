cmake_minimum_required(VERSION 3.5)

project(ll_qgis_layer_export VERSION 0.1 LANGUAGES CXX)

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

#QT5_WRAP_UI(ll_qgis_layer_export qgsvectorlayersaveasdialogbase.ui)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        exportdockwidget.h exportdockwidget.cpp exportdockwidget.ui
        qgsstatusbarscalewidget.h qgsstatusbarscalewidget.cpp
        qgsstatusbarcoordinateswidget.h qgsstatusbarcoordinateswidget.cpp
        qgsvectorlayersaveasdialogbase.ui
        vectorlayersaveasdialog.h vectorlayersaveasdialog.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ll_qgis_layer_export
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ll_qgis_layer_export APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ll_qgis_layer_export SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ll_qgis_layer_export
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(WIN32)
    target_link_libraries(ll_qgis_layer_export
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Svg
        ll_qgis_base_lib
        qgis_app.lib
        qgis_core.lib
        qgis_gui.lib
        qgis_analysis.lib)
ELSEIF(UNIX)
    target_link_libraries(ll_qgis_layer_export
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Svg
        ll_qgis_base_lib.so
        qgis_app.so
        qgis_core.so
        qgis_gui.so
        qgis_analysis.so)
ENDIF()

set_target_properties(ll_qgis_layer_export PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ll_qgis_layer_export)
endif()
