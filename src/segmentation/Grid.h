#ifndef PXSORT2_GRID_H
#define PXSORT2_GRID_H

#include "common.h"
#include "Segmentation.h"

class pxsort::Grid : public Segmentation {
public:
    /**
     * Create a new segmentation of the given Image into a grid of Rectangles.
     * @param img
     * @param rows How many rows in the grid, or 0 to use the height of img
     * @param columns How many columns in the grid, or 0 to use the width of img
     * @param x0 The x coordinate of the top-left corner of the grid.
     * @param y0 The y coordinate of the top-left corner of the grid.
     */
    Grid(const std::shared_ptr<Image>& img,
               int rows,
               int columns,
               int x0,
               int y0);

private:
    const int rows;
    const int columns;
    const int x0;
    const int y0;
    const int img_width;
    const int img_height;

    void initTiles(const std::shared_ptr<Image>& img);

    typedef std::array<int, 4> TileSpec;
    TileSpec tileSpec(int idx);
};



#endif //PXSORT2_GRID_H
