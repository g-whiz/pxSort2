cmake_minimum_required(VERSION 3.16)

project(pxsort2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/extern/computecpp-sdk/cmake/Modules/)
#find_package(ComputeCpp)

find_package(OpenMP REQUIRED)
#find_package(Boost 1.74 REQUIRED)
#find_package(OpenCV REQUIRED core imgproc)

#add_subdirectory(extern/pybind11)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}" -DPXSORT_DEBUG")
endif ()
add_subdirectory(src)
# add_subdirectory(test/qt-app-poc)
# add_subdirectory(python_bindings)