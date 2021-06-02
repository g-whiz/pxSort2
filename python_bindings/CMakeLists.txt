find_package(OpenCV REQUIRED core imgproc)
find_package(OpenMP REQUIRED)

add_library(python_bindings MODULE
        image.cpp
        segment.cpp
        effect.cpp
        comparator.cpp
        mixer.cpp
        segmentation.cpp
        pxsort.cpp
        core.cpp
        predicate.cpp
        projection.cpp wrapper.h)

add_dependencies(python_bindings pxsort)
target_include_directories(python_bindings PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(python_bindings PRIVATE
        pybind11::module
        pybind11::lto
        pybind11::windows_extras
        OpenMP::OpenMP_CXX
        ${OpenCV_LIBS}
        pxsort)

pybind11_extension(python_bindings)
pybind11_strip(python_bindings)

set_target_properties(python_bindings PROPERTIES
        CXX_VISIBILITY_PRESET "hidden"
        CUDA_VISIBILITY_PRESET "hidden"
        PREFIX ""
        OUTPUT_NAME "pxsort"
        SUFFIX ".so")

install(TARGETS python_bindings
        LIBRARY DESTINATION test/python)