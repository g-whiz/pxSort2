#ifndef PXSORT2_PREDICATE_H
#define PXSORT2_PREDICATE_H

#include "common.h"

namespace pxsort::predicate {

    /**
     * Create a PixelPredicate f that applies the linear transformation:
     *     f(p) = M*p + b
     *   to given pixels and returns the result.
     * @param M A linear map from [0, 1]^3 to RED in matrix form.
     * @param b A bias parameter
     * @return
     */
    PixelPredicate linear(const cv::Vec3f& M, float b);

    /**
     * Returns a PixelPredicate that returns true if and only if a pixel
     *   conforms to each of the specified thresholds.
     * @param rMin If set: the minimum acceptable value for the red channel.
     * @param rMax If set: the maximum acceptable value for the red channel.
     * @param gMin If set: the minimum acceptable value for the green channel.
     * @param gMax If set: the maximum acceptable value for the green channel.
     * @param bMin If set: the minimum acceptable value for the blue channel.
     * @param bMax If set: the maximum acceptable value for the blue channel.
     * @return
     */
    PixelPredicate threshold(std::optional<float> rMin={},
                             std::optional<float> rMax={},
                             std::optional<float> gMin={},
                             std::optional<float> gMax={},
                             std::optional<float> bMin={},
                             std::optional<float> bMax={});

}

#endif //PXSORT2_PREDICATE_H
