include(FindPython3)

find_package(Python3 COMPONENTS Interpreter Development)
find_package(Boost COMPONENTS Python)

python3_add_library(pxsort.py MODULE image.cpp segment.cpp effect.cpp)
add_dependencies(pxsort.py pxsort)
target_include_directories(pxsort.py PRIVATE ${CMAKE_SOURCE_DIR}/pxsort)