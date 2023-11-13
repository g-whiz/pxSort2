set(python_module pxsort)
pybind11_add_module(${python_module} MODULE
        python_bindings.cpp
        fwd.h
        util.h
        Sorter.h                Sorter.cpp
        Segment.h               Segment.cpp
        Image.h                 Image.cpp
        Map.h                   Map.cpp
        SegmentPixels.h         SegmentPixels.cpp
        Skew.h                  Skew.cpp
        geometry/Point.h
        geometry/Ellipse.h      geometry/Ellipse.cpp
        geometry/Polygon.h      geometry/Polygon.cpp
        geometry/Modulation.h   geometry/Modulation.cpp)
target_link_libraries(${python_module} PRIVATE TBB::tbb OpenMP::OpenMP_CXX)
target_include_directories(${python_module} PUBLIC
        ${CMAKE_SOURCE_DIR}/src/cpp
        # Eigen v3.4.0
        ${CMAKE_SOURCE_DIR}/lib/eigen)
target_compile_definitions(${python_module}
                           PRIVATE VERSION_INFO=${PROJECT_VERSION})

set_target_properties(${python_module} PROPERTIES
        OUTPUT_NAME "_native"
        SUFFIX ".so")

set(PYTHON_MODULE_INSTALL_DIR "${CMAKE_INSTALL_DIR}/python")
file(MAKE_DIRECTORY "${PYTHON_INSTALL_DIR}")
install(TARGETS ${python_module} DESTINATION ${PYTHON_MODULE_INSTALL_DIR})
