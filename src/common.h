#ifndef PXSORT2_COMMON_H
#define PXSORT2_COMMON_H

#include <concepts>
#include <opencv2/core/types.hpp>

/*
 * Common macros.
 */

/**
 * Function for non-negative modular arithmetic.
 */
#define PXSORT_MODULO(a, b)  (((a) % (b) + (b)) % (b))

/**
 * Safe base-2 uint32_t logarithm.
 */
#define PS_LOG_2(x)      (31 - __builtin_clz(x))

namespace pxsort {

    /* Forward declarations of library classes. */

    class Image;

    class Sorter;
    class PartialBubbleSort;
    class PartialHeapify;
    class BucketSort;

    class Segment;
    class Rectangle;
    class Asendorf;

    class Segmentation;
    class Grid;

    class PixelMixer;
    class LinearMixer;
    class ChannelCopier;

    class PixelPredicate;
    class LinearPredicate;
    class ThresholdPredicate;


    template<typename T, typename... ParamTypes>
    class Parameterization;

    template<typename T>
    using Static = Parameterization<T>;

    template<typename T>
    using TimeVarying = Parameterization<T, double>;

    template<typename T, size_t Dimension>
    using Field = Parameterization<T, cv::Vec<float, Dimension>>;


    /* Library typedefs. */

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
     * A PixelProjection is a projective map from [0, 1]^3 to [0, 1].
     *
     * It is used for applying a total ordering to pixels when performing a
     * BucketSort.
     */
    typedef std::function<float(const Pixel&)> PixelProjection;

    /** Matrix of the form [S_1 S_2 S_3], where each S_i is a vector of the form
     *    (dx, dy) defining the offset within a Segment for retrieving the ith
     *    channel of a pixel.
     *  This allows us to "skew" the channels of an image while applying
     *    effects. How skew is interpreted depends on the specific Segment
     *    implementation that is used.
     */
    typedef cv::Matx<int, 2, 3> ChannelSkew;

    /* Common enums/constants. */

    /**
     * Convenience function for creating a Skew with all 0 offsets.
     * @return
     */
    inline ChannelSkew NO_SKEW() {
        return {0, 0, 0,
                0, 0, 0};
    }

    /** Traversal options for a Segment's pixels. */
    enum SegmentTraversal {
        FORWARD,
        REVERSE,
        BINARY_TREE_BREADTH_FIRST
    };

    /** Indices of channels in Pixels by name. */
    enum Channel {
        RED = 0,
        GREEN = 1,
        BLUE = 2
    };

    /**
     * Constrain the
     * @tparam T Any type that supports the > and < operators.
     * @param x
     * @param lo
     * @param hi
     * @return
     */
    template<typename T>
    inline T clamp(T x, T lo, T hi) {
        return MIN(hi, MAX(lo, x));
    }

    template<typename T>
    T min(T a, T b) {
        return a < b ? a : b;
    }

    template<typename T, typename... Args>
    T min(T first, Args... rest) {
        T restMin = min(rest...);
        return first < restMin ? first : restMin;
    }

    template<typename T>
    T max(T a, T b) {
        return a >= b ? a : b;
    }

    template<typename T, typename... Args>
    T max(T first, Args... rest) {
        T restMin = min(rest...);
        return first >= restMin ? first : restMin;
    }

    template<typename Interface>
    class CloneableInterface {
    public:
        virtual std::unique_ptr<Interface> clone() const = 0;
    };

    template<typename Interface, typename Implementation>
    class CloneableImpl : public Interface {
        std::unique_ptr<Interface> clone() const override {
            return std::move(std::unique_ptr<Interface>(
                    new Implementation(*((Implementation *) this))));
        }
    };

}

#endif //PXSORT2_COMMON_H
