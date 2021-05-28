cmake_minimum_required(VERSION 3.18)

project(pxsort2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/pybind11)

add_subdirectory(src)
add_subdirectory(test/qt-app-poc)
add_subdirectory(python)
add_subdirectory(cython)