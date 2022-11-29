#ifndef PXSORT2_PIXELPROJECTION_H
#define PXSORT2_PIXELPROJECTION_H

#include "common.h"

/**
 * A PixelProjection is a projective map from [0, 1]^3 to [0, 1].
 *
 * It is used for applying a total ordering to pixels when performing a
 * BucketSort.
 */
class pxsort::PixelProjection : public CloneableInterface<PixelProjection> {
public:
    /**
     * Create a PixelProjection f that applies the linear transformation:
     *     f(p) = M*p + b
     *   to given pixels and returns the result clamped to [0, 1].
     * @param M A linear map from [0, 1]^3 to [0, 1] in matrix form.
     * @param b A bias parameter
     */
    PixelProjection(const cv::Vec3f &M, float b);

    /**
     * Create a PixelProjection that returns the specified channel for any
     *   given pixel.
     * @param channel The channel to projectPixel.
     */
    explicit PixelProjection(Channel channel);

    PixelProjection(const PixelProjection& other);

    float operator()(const Pixel&);

    [[nodiscard]] std::unique_ptr<PixelProjection> clone() const override;

private:
    const cv::Vec3f M;
    const float b;
};

#endif //PXSORT2_PIXELPROJECTION_H
