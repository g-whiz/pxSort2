cmake_minimum_required(VERSION 3.5)

project(pxsort2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED core)
find_package(OpenMP REQUIRED)

find_package(OpenCV REQUIRED core)
find_package(OpenMP REQUIRED)

include_directories(pxsort)

add_library(pxsort
        pxsort/common.h
        pxsort/Image.h
        pxsort/Image.cpp
        pxsort/Effect.cpp
        pxsort/Effect.h
        pxsort/effect/BubbleSort.h
        pxsort/effect/BubbleSort.cpp
        pxsort/effect/Heapify.h
        pxsort/effect/Heapify.cpp
        pxsort/Segment.cpp
        pxsort/Segment.h
        pxsort/Segmentation.h
        pxsort/Segmentation.cpp
        pxsort/segment/Rectangle.cpp
        pxsort/segment/Rectangle.h
        pxsort/segmentation/Grid.cpp
        pxsort/segmentation/Grid.h
        pxsort/Mixer.h
        pxsort/Mixer.cpp
        pxsort/Comparator.h
        pxsort/Comparator.cpp)

target_link_libraries(pxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})

add_subdirectory(test/qt-app-poc)