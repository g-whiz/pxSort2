cmake_minimum_required(VERSION 3.16)

project(pxsort2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(PXSORT_DEBUG)
endif ()
add_subdirectory(cpp)
add_subdirectory(swig)
