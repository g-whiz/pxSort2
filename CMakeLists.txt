cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(pxsort VERSION 0.0.1)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if(NOT DEFINED CMAKE_INSTALL_DIR)
    set(CMAKE_INSTALL_DIR "${CMAKE_SOURCE_DIR}/dist")
endif()
message(STATUS "install location: ${CMAKE_INSTALL_DIR}")


set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(PXSORT_DEBUG)
endif ()

# Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
include(GNUInstallDirs)
add_subdirectory(lib/pybind11)

include(cpp)
include(python)
