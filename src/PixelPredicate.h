#ifndef PXSORT2_PIXELPREDICATE_H
#define PXSORT2_PIXELPREDICATE_H

#include "common.h"

/**
 * A PixelPredicate is a predicate that evaluates whether some property of
 *   a pixel holds.
 * A PixelPredicate f maps a Pixel p to the real numbers such that for the
 *   equivalent predicate F on p, the following equivalency holds:
 *     f(p) >= 0   <==>    F(p) == true
 *     f(p) <  0   <==>    F(p) == false
 */
class pxsort::PixelPredicate {
public:
    PixelPredicate() = delete;

    /**
     * Creates a PixelPredicate that evaluates to true if and only
     *   if a pixel's channels are above/below the specified thresholds.
     * @param rMin The lower threshold for the red channel.
     * @param rMax The upper threshold for the red channel.
     * @param gMin The lower threshold for the green channel.
     * @param gMax The upper threshold for the green channel.
     * @param bMin The lower threshold for the blue channel.
     * @param bMax The upper threshold for the blue channel.
     */
    explicit PixelPredicate(float rMin = 0.0,
                            float rMax = 1.0,
                            float gMin = 0.0,
                            float gMax = 1.0,
                            float bMin = 0.0,
                            float bMax = 1.0);

    /**
     * Creates a PixelPredicate derived from the linear transformation:
     *     f(p) = proj * p + b
     * @param proj A linear map from [0, 1]^3 to [0, 1] in matrix form.
     * @param bias A bias parameter
     */
    PixelPredicate(const cv::Vec3f& proj, float bias);

    PixelPredicate(const PixelPredicate &other);

    /**
     * Apply this PixelPredicate to the given pixel.
     */
    float operator()(const Pixel&) const;

    PixelPredicate operator&&(const PixelPredicate &other) const;

    PixelPredicate operator||(const PixelPredicate &other) const;

    PixelPredicate operator!() const;

private:
    class Impl;
    class LinearImpl;
    class ThresholdImpl;
    class CombinationImpl;
    class NegationImpl;

    explicit PixelPredicate(Impl *pImpl);

    const std::unique_ptr<Impl> pImpl;
};

#endif //PXSORT2_PIXELPREDICATE_H
