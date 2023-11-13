#ifndef PXSORT2_MAP_H
#define PXSORT2_MAP_H

#include <functional>
#include <cstdint>
#include <memory>
#include "fwd.h"

namespace pxsort {

    /**
     * A map from R^inDim to R^nPixels.
     */
    class Map {
    public:
        class MapImpl;

        using fp_t = void(*)(const float *, int32_t, float *, int32_t);
        using fn_t = std::function<void(const float *, int32_t,
                                        float *, int32_t)>;

        /**
         * The dimension of this Map's input.
         */
        const int32_t inDim;

        /**
         * The dimension of this Map's output.
         */
        const int32_t outDim;

        /**
         * Copy constructor.
         * @param other
         */
        Map(const Map &other) = default;

        /**
         * Move constructor.
         * @param other
         */
        Map(Map &&other) = default;

        /**
         * Creates a new Map from a function pointer.
         *
         * NOTE: this constructor is intended for use with runtime-defined
         *   functions.
         * In particular, the main motivation for this method of instantiating
         *   Maps is to enable the use of a JIT compilation engine (like Numba) to
         *   allow calls to dynamically-compiled numerical functions.
         *
         * @param fp The function pointer to use for this map.
         * This function's arguments are treated as:
         *     (float *in_ptr, int in_size, float* out_ptr, int out_size)
         * Each of in_ptr and out_ptr can be assumed to be initialized float
         *   arrays of length in_size and out_size respectively.
         * The input for this Map is passed to fp via in_ptr, and fp is expected to
         *   return it's result via out_ptr, which this map will subsequently return
         *   as its result.
         * @param in_dim The dimension of the input to this map (and to fp).
         * @param out_dim The dimension of the output from this map (and from fp).
         */
        Map(fp_t fp, int32_t in_dim, int32_t out_dim);

        /**
         * Creates a new Map from a std::function object.
         * @param fn The std::function to use for this map.
         * @param in_dim The dimension of the input to this map (and to fp).
         * @param out_dim The dimension of the output from this map (and from fp).
         */
        Map(fn_t fn, int32_t in_dim, int32_t out_dim);

        /**
         * Function composition operator.
         * Let fp: B -> C, g: A -> B.
         * Then (fp << g): A -> C.
         * @param that A Map whose nPixels equals this Map's inDim
         * @throws std::invalid_argument If the dimension constraints are violated.
         * @return A Map that is the composition of the given Map and this Map.
         */
        Map operator<<(const Map &that) const;

        /**
         * Function concatenation operator.
         * Let fp: A -> B, g: C -> D.
         * Then (fp | g): A x C -> B x D.
         * @param that A Map
         * @return A Map that is the composition of the given Map and this Map.
         */
        Map operator|(const Map &that) const;

        /**
         * Concatenation operator for a variable number of maps.
         * @param maps
         * @return
         */
        static Map concatenate(const std::vector<Map> &maps);

        /**
         * Returns a Map with inDim == in_dim, n == c.size() that always evaluates to
         * the values contained in c.
         * This can be used to achieve currying (i.e. parameter binding).
         * @param c The constant values for the resulting map to evaluate to.
         * @return
         */
        static Map constant(std::vector<float> c, int32_t in_dim = 0);

        /**
         * Function "fork" operator.
         * Let fp: A -> B, g: A -> C
         * Then (fp ^ g): A -> B x C
         * i.e. the resulting map returns the concatenated result of fp and g when
         *      evaluated on the same input
         * @param that A Map whose inDim equals this Map's inDim
         * @throws std::invalid_argument If the dimension constraints are violated.
         * @return
         */
        Map operator^(const Map &that) const;

        /**
         * Projection operator.
         * Produces the ith projection of this Map.
         * @param i An integer in the range [0, ... , nPixels - 1]
         * @return The ith projection of this map.
         */
        Map operator[](int i) const;

        /**
         * Invoke this Map on the given vector.
         * WARNING: runtime behaviour is not guaranteed if the given vector's size
         *   does not match this Map's inDim.
         * @param x A vector<float> with size == inDim.
         * @return The resulting vector<float> with size() == nPixels
         */
        std::vector<float> operator()(const std::vector<float> &x) const;

        /**
         * Invoke this map using C array and pointer-return semantics.
         * WARNING: this function is unsafe! The caller is responsible for
         *     ensuring that the provided arrays are valid, and properly
         *     initialized.
         * @param in A C array containing the input to this Map.
         *     Assumed to be allocated and initialized with length inDim.
         * @param out A C array to return this Map's output via.
         *     Assumed to be allocated with length nPixels.
         */
        void operator()(const float *in, float *out) const;

        bool operator==(const Map &other) const;

    private:
        Map(std::shared_ptr<MapImpl> pImpl, int32_t in_dim, int32_t out_dim);

        const std::shared_ptr<MapImpl> pImpl;
    };
}
#endif //PXSORT2_MAP_H
