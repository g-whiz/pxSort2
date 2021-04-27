#ifndef PXSORT2_TILE_H
#define PXSORT2_TILE_H

#include "Image.h"
#include "Effect.h"

/**
 * Function for non-negative modular arithmetic.
 */
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
     *
     * The primary purpose of the Tile interface is to provide Effects with a
     *   1-dimensional array-like view of some subset of an image's pixels.
     * In this sense, a tile maps an arbitrary subset of an Image's pixels to
     *   a virtual array.
     *
     * Implementing classes must implement the forwardGetPixel, and
     *   forwardSetPixel methods, which provide the virtual array semantics
     *   used by Effects.
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

        /**
         * Associates the given Effect with this Tile.
         *
         * @param e The effect to attach.
         */
        void attach(std::unique_ptr<Effect> e);

        /**
         * Applies all Effects attached to this Tile once.
         * In particular, for each Effect e associated with this Tile t, this
         *   function calls e.apply(t).
         *
         * Note that this function will apply effects in the order that they
         *   were attached.
         */
        void applyEffects();

    protected:
        /**
         * Retrieves a Pixel from this Tile's underlying Image using a FORWARD
         *   traversal strategy.
         * @param idx Index of the Pixel.
         * @param skew Skew of the Pixel
         * @return
         */
        virtual Pixel forwardGetPixel(int idx,
                                      ChannelSkew &skew) = 0;

        /**
         * Stores a Pixel from this Tile's underlying Image using a FORWARD
         *   traversal strategy.
         * @param idx Index of the Pixel.
         * @param skew Skew of the Pixel
         * @return
         */
        virtual void forwardSetPixel(int idx,
                                     ChannelSkew &skew,
                                     const Pixel &px) = 0;

        Tile(std::weak_ptr<Image> &img);

        /** Temporary strong reference to the underlying */
        std::shared_ptr<Image> img;

        /**
         * Acquires a shared_ptr to the Image referenced by weakImg and stores
         *   it in img. Returns true iff the shared_ptr was acquired
         *   successfully.
         * @return
         */
        bool acquireImg();

        /**
         * Clears the shared_ptr acquired by acquireImg().
         */
        void releaseImg();

    private:
        int getForwardIndex(int idx, Traversal t);
        int btbfToForwardIdx(int idx);

        std::vector<std::unique_ptr<Effect>> effects;

        /** Weak reference to the Image containing the underlying data
         *    referenced by this tile. */
        std::weak_ptr<Image> weakImg;
    };
}


#endif //PXSORT2_TILE_H
