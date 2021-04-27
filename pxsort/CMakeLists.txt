find_package(OpenCV REQUIRED core)
find_package(OpenMP REQUIRED)

include_directories(include)

add_library(libpxsort
        include/pxsort.h
        src/Image.h
        src/Image.cpp
        src/Effect.cpp
        src/Effect.h
        src/Tile.cpp
        src/Tile.h
        src/Tiling.h
        src/Tiling.cpp
        src/RectangleTile.cpp
        src/RectangleTile.h
        src/GridTiling.cpp
        src/GridTiling.h)

target_link_libraries(libpxsort OpenMP::OpenMP_CXX ${OpenCV_LIBS})