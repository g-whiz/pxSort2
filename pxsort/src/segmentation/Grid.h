#ifndef PXSORT2_GRID_H
#define PXSORT2_GRID_H

#include "src/Segmentation.h"

namespace ps {

    class Grid : public Segmentation {
    public:
        Grid(const std::shared_ptr<Image>& img,
                   int rows,
                   int columns,
                   int x0,
                   int y0);

        int size() override;

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
}


#endif //PXSORT2_GRID_H
