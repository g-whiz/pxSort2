#ifndef PXSORT2_COMMON_H
#define PXSORT2_COMMON_H

#include <memory>

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

    class Image;

    class Sorter;

    class Segment;
    class SegmentPixels;

    class Map;

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
}

#endif //PXSORT2_COMMON_H
