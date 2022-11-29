


add_library(pxsort
    common.h
    Sorter.h
    Sorter.cpp
    Segment.h
    Segment.cpp
    Image.h
    Image.cpp
    Map.h
    Map.cpp
)

set_property(TARGET pxsort PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(pxsort PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(pxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})
