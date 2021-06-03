cmake_minimum_required(VERSION 3.16)

project(pxsort2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/extern/computecpp-sdk/cmake/Modules/)
find_package(ComputeCpp)

add_subdirectory(extern/pybind11)


add_subdirectory(src)
add_subdirectory(test/qt-app-poc)
add_subdirectory(python_bindings)