cmake_minimum_required(VERSION 3.14)

project(qgis_cpp_api_apps VERSION "1.0.0.0")
project(qgis_cpp_api_apps LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/inc) # 指定头文件搜索路径

if(WIN32)
    MESSAGE(STATUS "Now is windows")
#    add_compile_options(/W4)
    add_compile_options(/wd4819)
    add_definitions(-D_USE_MATH_DEFINES)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/windows/qgis-ltr-dev/include)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/windows/gdal/include)
    link_directories(${PROJECT_SOURCE_DIR}/thirdparty/windows/qgis-ltr-dev/lib/) # 指定库搜索路径
ELSEIF(UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/linux/gdal/include)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/linux/qgis/include/qgis)
    link_directories(${PROJECT_SOURCE_DIR}/thirdparty/linux/qgis/lib/) # 指定库搜索路径
ENDIF()

link_directories(${PROJECT_SOURCE_DIR}/bin) # 指定库搜索路径

 # 指定 编译子目录
add_subdirectory(./src/ll_qgis_base_lib)
add_subdirectory(./src/ll_qgis_hello)
add_subdirectory(./src/ll_qgis_code_examples)
add_subdirectory(./src/ll_qgis_uav_realtime)
add_subdirectory(./src/ll_qgis_layer_export)
