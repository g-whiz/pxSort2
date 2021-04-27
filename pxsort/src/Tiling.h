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
         * Attaches the given effect to all tiles in this tiling.
         * @param e
         */
        void addEffect(std::unique_ptr<Effect> e);

        /**
         * Attaches the given effect to the specified tile in this tiling.
         * @param e
         * @param tile
         */
        void addEffect(std::unique_ptr<Effect> e, int tile);

        /**
         * Applies the effects attached to each tile in this tiling once.
         * In particular, this method calls Tile::applyEffects() on each of this
         *   tiling's tiles.
         */
        void applyEffects();

    protected:
        /** The tiles in this tiling. */
        std::vector<std::unique_ptr<Tile>> tiles;

    };

}


#endif //PXSORT2_TILING_H
