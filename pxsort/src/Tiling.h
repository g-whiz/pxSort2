#ifndef PXSORT2_TILING_H
#define PXSORT2_TILING_H

#include "Tile.h"

namespace ps {

    class Tiling {
    public:
        /**
         * Returns the number of Tiles in this Tiling.
         */
        virtual int size() = 0;

        /**
         * Returns the specified Tile in this Tiling.
         * @param idx
         */
        virtual Tile getTile(int idx) = 0;

    };

}


#endif //PXSORT2_TILING_H
