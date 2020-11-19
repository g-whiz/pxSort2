add_library(libpxsort
        Image.h Image.cpp Effect.cpp Effect.h Tile.cpp Tile.h Tiling.cpp Tiling.h)

target_link_libraries(libpxsort ${OpenCV_LIBS})