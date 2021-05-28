list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/extern/scikit-build/skbuild/resources/cmake")
find_package(PythonExtensions REQUIRED)

python_extension_module(pxsort)