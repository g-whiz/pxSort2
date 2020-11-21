add_library(libpxsort
        Image.h Image.cpp Effect.cpp Effect.h Tile.cpp Tile.h Tiling.h RectangleTile.cpp RectangleTile.h)

target_link_libraries(libpxsort ${OpenCV_LIBS})