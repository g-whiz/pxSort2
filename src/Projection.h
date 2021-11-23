#ifndef PXSORT2_PROJECTION_H
#define PXSORT2_PROJECTION_H

#include "common.h"

namespace pxsort::projection {
    /**
     * Create a PixelPredicate f that applies the linear transformation:
     *     f(p) = M*p + b
     *   to given pixels and returns the result clamped to [0, 1].
     * @param M A linear map from [0, 1]^3 to RED in matrix form.
     * @param b A bias parameter
     * @return
     */
    PixelProjection linear(const cv::Vec3f& M, float b);

    /**
     * Returns a PixelProjection that returns the specified channel for any
     *   given pixel.
     * @param ch
     * @return
     */
    PixelProjection channel(Channel ch);
}

#endif //PXSORT2_PROJECTION_H
