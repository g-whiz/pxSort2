#include "Grid.h"
#include "Image.h"
#include "segment/Rectangle.h"


using namespace pxsort;

Grid::Grid(const std::shared_ptr<Image>& img,
                       int rows, int columns,
                       int x0, int y0)
    : Segmentation(),
      rows(rows < 1 ? img->height : rows),
      columns(columns < 1 ? img->width : columns),
      x0(x0), y0(y0),
      img_width(img->width), img_height(img->height){
    assert(this->rows > 0);
    assert(this->columns > 0);
    initTiles(img);
}


Grid::TileSpec Grid::tileSpec(int idx) {
    int row = idx / columns;
    int col = PXSORT_MODULO(idx, columns);

    int width_residue = PXSORT_MODULO(img_width, columns);
    int height_residue = PXSORT_MODULO(img_height, rows);

    int tile_width = img_width / columns + (col < width_residue ? 1 : 0);
    int tile_height = img_height / rows + (row < height_residue ? 1 : 0);

    // NOTE: we use the torus topology here for "wrap arounds"
    // TODO: implement more general topology code (supporting torus,
    //       x/y klein bottles, real projective plane)
    int tile_x0 = PXSORT_MODULO(x0 + col * (img_width / columns)
                                + MAX(0, MIN(col, width_residue)),
                                img_width);
    int tile_y0 = PXSORT_MODULO(y0 + row * (img_height / rows)
                                + MAX(0, MIN(row, height_residue - 1)),
                                img_height);

    return {tile_width, tile_height, tile_x0, tile_y0};
}

void Grid::initTiles(const std::shared_ptr<Image>& img) {
    for (int idx = 0; idx < rows * columns; idx++){
        auto[tile_width, tile_height, tile_x0, tile_y0] = tileSpec(idx);

        std::unique_ptr<Segment> tile = std::make_unique<Rectangle>(
                img, tile_width, tile_height, tile_x0, tile_y0);
        segments.push_back(std::move(tile));
    }
}
