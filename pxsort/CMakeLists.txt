find_package(OpenCV REQUIRED core)
find_package(OpenMP REQUIRED)

include_directories(include)

add_library(libpxsort
        include/pxsort.h
        src/Image.h
        src/Image.cpp
        src/Effect.cpp
        src/Effect.h
        src/effect/CompareAndMix.h
        src/effect/CompareAndMix.cpp
        src/effect/BubbleSort.h
        src/effect/BubbleSort.cpp
        src/effect/Heapify.h
        src/effect/Heapify.cpp
        src/Segment.cpp
        src/Segment.h
        src/Segmentation.h
        src/Segmentation.cpp
        src/segment/Rectangle.cpp
        src/segment/Rectangle.h
        src/segmentation/Grid.cpp
        src/segmentation/Grid.h)

target_link_libraries(libpxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})