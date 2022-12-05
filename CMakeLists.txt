cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(pxsort VERSION 0.0.1)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if(NOT DEFINED CMAKE_INSTALL_DIR)
    set(CMAKE_INSTALL_DIR "${CMAKE_SOURCE_DIR}/dist")
endif()
message(STATUS "install location: ${CMAKE_INSTALL_DIR}")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(PXSORT_DEBUG)
endif ()

# Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
include(GNUInstallDirs)
include(FetchContent)

# Fetch pybind11 lib (v2.10.1)
FetchContent_Declare(
        pybind11
        URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.1.tar.gz
        URL_HASH SHA256=111014b516b625083bef701df7880f78c2243835abdb263065b6b59b960b6bad
)
FetchContent_MakeAvailable(pybind11)

include(cpp)
include(python)
