find_package(OpenCV REQUIRED core imgproc)
find_package(OpenMP REQUIRED)

add_library(pxsort
        common.h
        Image.h
        Image.cpp
        Sorter.cpp
        Sorter.h
        sorter/PartialBubbleSort.h
        sorter/PartialBubbleSort.cpp
        sorter/PartialHeapify.h
        sorter/PartialHeapify.cpp
        Segment.cpp
        Segment.h
        Segmentation.h
        Segmentation.cpp
        segment/Rectangle.cpp
        segment/Rectangle.h
        segmentation/Grid.cpp
        segmentation/Grid.h
        PixelMixer.h
        PixelComparator.h
        PixelComparator.cpp
        segment/Asendorf.cpp
        segment/Asendorf.h
        sorter/BucketSort.cpp
        sorter/BucketSort.h
        PixelProjection.h
        PixelPredicate.h
        PixelProjection.cpp
        mixer/ChannelCopier.h
        mixer/ChannelCopier.cpp
        mixer/LinearMixer.h
        mixer/LinearMixer.cpp
        Parameterization.h
        predicate/LinearPredicate.cpp
        predicate/LinearPredicate.h
        predicate/ThresholdPredicate.cpp
        predicate/ThresholdPredicate.h
        PixelMixer.cpp
        PixelPredicate.cpp)

set_property(TARGET pxsort PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(pxsort PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(pxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})
