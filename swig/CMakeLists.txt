find_package(Python COMPONENTS Development Interpreter)

if (Python_FOUND)
    message(${Python_SITELIB}) # print sitepath
else()
    message("No Python found.")
    return()
endif()

find_package(SWIG)
include(UseSWIG)

if(SWIG_FOUND)
    message("Found SWIG ${SWIG_VERSION}")
else()
    message("No SWIG found.")
    return()
endif()

set(UseSWIG_TARGET_NAME_PREFERENCE STANDARD)

set_property(SOURCE pxsort.i PROPERTY CPLUSPLUS ON)

include_directories("${CMAKE_SOURCE_DIR}/cpp" "${Python_INCLUDE_DIRS}")

