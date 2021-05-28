find_package(OpenCV REQUIRED core imgproc)
find_package(OpenMP REQUIRED)

add_library(pxsort.py MODULE
        image.cpp
        segment.cpp
        effect.cpp
        comparator.cpp
        mixer.cpp
        segmentation.cpp pxsort.cpp core.cpp predicate.cpp projection.cpp)

add_dependencies(pxsort.py pxsort)
target_include_directories(pxsort.py PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(pxsort.py PRIVATE
        pybind11::module
        pybind11::lto
        pybind11::windows_extras
        OpenMP::OpenMP_CXX
        ${OpenCV_LIBS}
        pxsort)

pybind11_extension(pxsort.py)
pybind11_strip(pxsort.py)

set_target_properties(pxsort.py PROPERTIES CXX_VISIBILITY_PRESET "hidden"
                                CUDA_VISIBILITY_PRESET "hidden")
