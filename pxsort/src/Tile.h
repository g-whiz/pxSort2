#ifndef PXSORT2_TILE_H
#define PXSORT2_TILE_H

#include "Image.h"

#define MODULO(a, b)  (((a) % (b) + (b)) % (b))
#define LOG_2(x)      (31 - __builtin_clz(x))

namespace ps {

    /** Matrix of the form [S_1 S_2 S_3], where each S_i is a vector of the form
     *    (dx, dy) defining the offset for retrieving the ith channel of a
     *    pixel. This allows us to "skew" the channels of an image while
     *    applying effects.
     */
    typedef cv::Matx<int, 2, 3> ChannelSkew;

    static const ChannelSkew NO_SKEW(0, 0, 0,
                                     0, 0, 0);

    /**
     * Interface for accessing a segment of an image.
     */
    class Tile {
    public:
        /** Traversal options for a Tile's pixels. */
        enum Traversal {
            FORWARD,
            REVERSE,
            BINARY_TREE_BREADTH_FIRST
        };

        /** Returns the number of pixels in this Tile. */
        virtual
        int size() = 0;

        /**
         * Returns the skewed pixel at the given index according to the
         *   specified Traversal.
         * @param idx Index of the pixel in this Tile.
         * @param t Method of traversing the pixels in this Tile.
         * @param skew Skew to use when retrieving the specified pixel.
         */
        Pixel getPixel(int idx, Traversal t, ChannelSkew skew);

        /**
         * Sets the skewed pixel at the given index according to the
         *    specified Traversal.
         * @param idx Index of the pixel in this Tile.
         * @param t Method of traversing the pixels in this Tile.
         * @param skew Skew to use when setting the specified pixel.
         * @param px The Pixel to store in the specified index.
         */
        void setPixel(int idx, Traversal t, ChannelSkew skew, const Pixel &px);

    private:
        virtual Pixel forwardGetPixel(int idx, ChannelSkew &skew) = 0;
        virtual void forwardSetPixel(int idx, ChannelSkew &skew, Pixel &px) = 0;

        int getForwardIndex(int idx, Traversal t);
        int btbfToForwardIdx(int idx);
    };
}


#endif //PXSORT2_TILE_H
