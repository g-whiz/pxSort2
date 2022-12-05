set(python_module pxsort)
pybind11_add_module(${python_module} MODULE
        python_bindings.cpp
        common.h
        Sorter.h
        Sorter.cpp
        Segment.h
        Segment.cpp
        Image.h
        Image.cpp
        Map.h
        Map.cpp)
target_include_directories(${python_module} PUBLIC ${CMAKE_SOURCE_DIR}/cpp)
target_compile_definitions(${python_module}
                           PRIVATE VERSION_INFO=${PROJECT_VERSION})

set_target_properties(${python_module} PROPERTIES
        OUTPUT_NAME "_native"
        SUFFIX ".so")

set(PYTHON_MODULE_INSTALL_DIR "${CMAKE_INSTALL_DIR}/python")
file(MAKE_DIRECTORY "${PYTHON_INSTALL_DIR}")
install(TARGETS ${python_module} DESTINATION ${PYTHON_MODULE_INSTALL_DIR})
