#ifndef PXSORT_UTIL_H
#define PXSORT_UTIL_H

#include <bit>

namespace pxsort {

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
        return std::min(hi, std::max(lo, x));
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

    /**
     * Function for modular arithmetic with non-negative return values.
     * @tparam T
     * @param x Dividend
     * @param m Modulus
     * @return Integer in the range [0, 1, ..., m - 1].
     */
    template <typename T>
    inline constexpr T modulo(T x, T m) {
        return (((x) % (m) + (m)) % (m));
    }

    /**
     * Efficient base-2 logarithm for positive integers.
     * @tparam T
     * @param x A positive integer.
     * @return The floor of the base 2 log of x.
     */
     template <typename T>
     inline constexpr T log2(T x) {
        #ifdef PXSORT_DEBUG
        assert(x > 0);
        #endif

        return std::bit_width(static_cast<std::make_unsigned<T>::type>(x)) - 1;
     }

     template <typename T>
     inline constexpr T deg2rad(T degrees) {
         return (2.0 * std::numbers::pi) * (degrees / 360.0);
     }
}
#endif //PXSORT_UTIL_H
