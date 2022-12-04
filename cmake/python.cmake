cmake_minimum_required(VERSION 3.18)

## Find SWIG
#set(CMAKE_SWIG_FLAGS)
#find_package(SWIG REQUIRED)
#include(UseSWIG)

# Find Python 3
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

#find_package(pybind11 CONFIG REQUIRED)
#
#set(PYTHON_PROJECT pxsort)
#message(STATUS "Python project: ${PYTHON_PROJECT}")
#set(PYTHON_PROJECT_DIR ${PROJECT_BINARY_DIR}/swig/python/${PYTHON_PROJECT})
#message(STATUS "Python project build path: ${PYTHON_PROJECT_DIR}")
#
#add_subdirectory(swig/python)
