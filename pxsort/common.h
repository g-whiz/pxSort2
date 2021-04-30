#ifndef PXSORT2_COMMON_H
#define PXSORT2_COMMON_H

#include <opencv2/core/types.hpp>

/*
 * Forward declarations, typedefs, macros.
 */

/**
 * Function for non-negative modular arithmetic.
 */
#define MODULO(a, b)  (((a) % (b) + (b)) % (b))

/**
 * Safe base-2 uint32_t logarithm.
 */
#define LOG_2(x)      (31 - __builtin_clz(x))

namespace ps {

    class Image;

    class Effect;
    class CompareAndMix;
    class BubbleSort;
    class Heapify;

    class Segment;
    class Rectangle;

    class Segmentation;
    class Grid;

    /**
     * Pixels are always points in the unit cube, [0, 1]^3.
     */
    typedef cv::Vec3f Pixel;

    /**
     * A PixelComparator is a callable object that imposes some ordering on
     *   Pixels.
     * The value returned by a PixelComparator is negative, zero, or positive
     *   when the first pixel is less-than, equal-to, or greater-than the second
     *   pixel, respectively.
     */
    typedef std::function<float(const Pixel&, const Pixel&)> PixelComparator;

    /**
     * A PixelMixer is a callable object that mixes/transforms two pixels to
     *   produce two new pixels.
     * In this sense, a PixelMixer can be considered an endomorphism on the set
     *   [0, 1]^6.
     */
    typedef std::function<std::pair<Pixel, Pixel>(const Pixel&, const Pixel&)>
            PixelMixer;

    /** Matrix of the form [S_1 S_2 S_3], where each S_i is a vector of the form
     *    (dx, dy) defining the offset within a Segment for retrieving the ith
     *    channel of a pixel.
     *  This allows us to "skew" the channels of an image while applying
     *    effects. How skew is interpreted depends on the specific Segment
     *    implementation that is used.
     */
    typedef cv::Matx<int, 2, 3> ChannelSkew;

    /**
     * Convenience function for creating a Skew with all 0 offsets.
     * @return
     */
    inline ChannelSkew NO_SKEW() {
        return ChannelSkew(0, 0, 0,
                           0, 0, 0);
    }

    /** Traversal options for a Segment's pixels. */
    enum SegmentTraversal {
        FORWARD,
        REVERSE,
        BINARY_TREE_BREADTH_FIRST
    };
}

#endif //PXSORT2_COMMON_H
