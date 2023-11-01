cmake_minimum_required(VERSION 3.14)

project(ll_qgis_base_lib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/../../bin)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Xml REQUIRED)

add_library(ll_qgis_base_lib SHARED
  ll_qgis_base_lib_global.h
  ll_qgis_base_lib_singleton.h
  ll_qgis_base_lib.cpp ll_qgis_base_lib.h
  ll_qgis_base_lib_layerhandling.cpp ll_qgis_base_lib_layerhandling.h
  ll_qgis_base_lib_layertreeview_menu.cpp ll_qgis_base_lib_layertreeview_menu.h
  qgsstatusbarscalewidget.h qgsstatusbarscalewidget.cpp
  qgsstatusbarcoordinateswidget.h qgsstatusbarcoordinateswidget.cpp
  image.qrc
)
if(WIN32)
    target_link_libraries(ll_qgis_base_lib PRIVATE
        Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml
        qgis_core.lib
        qgis_gui.lib
        qgis_analysis.lib
        )
ELSEIF(UNIX)
    target_link_libraries(ll_qgis_base_lib PRIVATE
        Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml
    )
ENDIF()
target_compile_definitions(ll_qgis_base_lib PRIVATE LD_LIB_APP_LIBRARY)
