find_package(OpenCV REQUIRED core imgproc)
find_package(OpenMP REQUIRED)

add_library(pxsort
        common.h
        Image.h
        Image.cpp
        Effect.cpp
        Effect.h
        effect/PartialBubbleSort.h
        effect/PartialBubbleSort.cpp
        effect/PartialHeapify.h
        effect/PartialHeapify.cpp
        Segment.cpp
        Segment.h
        Segmentation.h
        Segmentation.cpp
        segment/Rectangle.cpp
        segment/Rectangle.h
        segmentation/Grid.cpp
        segmentation/Grid.h
        Mixer.h
        Mixer.cpp
        Comparator.h
        Comparator.cpp
        segment/Asendorf.cpp
        segment/Asendorf.h
        effect/BucketSort.cpp
        effect/BucketSort.h
        Projection.h
        Predicate.h Predicate.cpp Projection.cpp)

set_property(TARGET pxsort PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(pxsort PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(pxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})
