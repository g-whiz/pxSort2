find_package(OpenCV REQUIRED core)
find_package(OpenMP REQUIRED)

include_directories(include)

add_library(libpxsort
        include/pxsort.h
        pxsort/Image.h
        pxsort/Image.cpp
        pxsort/Effect.cpp
        pxsort/Effect.h
        pxsort/effect/CompareAndMix.h
        pxsort/effect/CompareAndMix.cpp
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
        pxsort/Mixer.cpp pxsort/Comparator.h pxsort/Comparator.cpp)

target_link_libraries(libpxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})