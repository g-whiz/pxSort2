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
#define PXSORT_LOG_2(x)      (31 - __builtin_clz(x))

namespace pxsort {

    /**
     * A type trait whose value is true iff the given trait is true for all
     *   given types.
     * @tparam Trait The trait to evaluate.
     * @tparam T One or more types to evaluate the trait on.
     */
    template<template<typename> typename Trait, typename... T> struct all;

    template<template<typename> typename Trait,
            typename First,
            typename Second,
            typename... Rest>
    struct all<Trait, First, Second, Rest...> : std::integral_constant<bool,
            Trait<First>::value && all<Trait, Second, Rest...>::value> {};

    template<template<typename> typename Trait, typename T>
    struct all<Trait, T> : std::integral_constant<bool, Trait<T>::value> {};

    /**
     * A type trait whose value is true if all of the given integers are valid
     * numbers of pixel valid_dimensions.
     * @tparam dimensions dimension sizes to check the validity of
     */
    template <int... dimensions> struct is_valid_dimensions;

    template <int first, int second, int... rest>
    struct is_valid_dimensions<first, second, rest...> :
        std::integral_constant<bool,
            (0 < first)
                && (first < PXSORT_MAX_DIMENSIONS)
                && is_valid_dimensions<second, rest...>::value> {};

    template <int dimension>
    struct is_valid_dimensions<dimension> :
        std::integral_constant<bool,
            (0 < dimension)
                && (dimension < PXSORT_MAX_DIMENSIONS)> {};


    /* Concepts for template parameter constraints. */
    template <typename... T>
    concept arithmetic = all<std::is_arithmetic, T...>::value;

    template <typename... T>
    concept floating_point = all<std::is_floating_point, T...>::value;

    template <typename... T>
    concept integral = all<std::is_integral, T...>::value;

    /**
     * True if each the specified dimension sizes is valid (i.e.
     *   positive and no greater than PXSORT_MAX_DIMENSIONS).
     * @tparam dims dimension sizes to check
     */
    template <int... dims>
    concept valid_dimensions = is_valid_dimensions<dims...>::value;

    /* Forward declarations of library classes. */
//    namespace map {
//
//        template <typename T, int m, int nPixels, typename S = T>
//        requires arithmetic<T, S> && valid_dimensions<m, nPixels>
//        class Map;
//
//    }

    class Image;

    template <int cn> requires valid_dimensions<cn>
    using Pixel = cv::Vec<float, cn>;

    class Sorter;

    class Segment;
    class SegmentPixels;

    class Map;

    /**
     * A per-channel coordinate transform.
     * Has the form: (x, y, channel) -> (x', y'), where x' and y' are offset
     *   coordinates at which to access the specified channel.
     */
//    using ChannelSkew = map::Map<int, 3, 2>;

    /**
     * A predicate on a pixel. Non-negative outputs are "true",
     * and negative outputs are "false".
     */
//    template<int cn> requires valid_dimensions<cn>
//    using PixelPredicate = map::Map<float, cn, 1>;

    /* DSL Classes */

//    template<typename T, size_t D>
//    class Selector;

//    using ChannelSelector = Selector<float, 3>;

    /* Library typedefs. */

    /** Matrix of the form [S_1 S_2 S_3], where each S_i is a vector of the form
     *    (dx, dy) defining the offset within a Segment for retrieving the ith
     *    channel of a pixel.
     *  This allows us to "skew" the cn of an image while applying
     *    effects. How skew is interpreted depends on the specific Segment
     *    implementation that is used.
     */
//    typedef cv::Matx<int, 2, 3> ChannelSkew;

//    typedef std::function<float(const Pixel&)>
//            PixelProjection;
//    typedef std::function<float(const Pixel&)>
//            PixelPredicate;


//    typedef std::function<std::pair<Pixel, Pixel>(const Pixel&, const Pixel&)>
//            PixelMixer;
//
//    typedef std::function<float(const Pixel&, const Pixel&)> PixelComparator;

//    typedef std::function<float(uint32_t, uint32_t)> CoordinateMap;


    /* Common enums/constants. */

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
        virtual ~CloneableInterface<Interface>() = default;
    };

    template<typename Interface, typename Implementation>
    class CloneableImpl : public Interface {
    private:
        std::unique_ptr<Interface> clone() const override {
            return std::move(std::unique_ptr<Interface>(
                    new Implementation(*((Implementation *) this))));
        }
    };
}

#endif //PXSORT2_COMMON_H
