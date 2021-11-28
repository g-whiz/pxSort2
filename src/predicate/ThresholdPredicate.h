#ifndef PXSORT2_THRESHOLDPREDICATE_H
#define PXSORT2_THRESHOLDPREDICATE_H

#include "common.h"
#include "PixelPredicate.h"


/**
 * A threshold predicate is a PixelPredicate that evaluates to true if and only
 *   if a pixel's channels are above/below some specified thresholds.
 */
class pxsort::ThresholdPredicate :
        public CloneableImpl<PixelPredicate, ThresholdPredicate> {
public:
    /**
     * @param rMin The minimum acceptable value for the red channel.
     * @param rMax The maximum acceptable value for the red channel.
     * @param gMin The minimum acceptable value for the green channel.
     * @param gMax The maximum acceptable value for the green channel.
     * @param bMin The minimum acceptable value for the blue channel.
     * @param bMax The maximum acceptable value for the blue channel.
     */
    explicit ThresholdPredicate(float rMin = 0.0,
                                float rMax = 1.0,
                                float gMin = 0.0,
                                float gMax = 1.0,
                                float bMin = 0.0,
                                float bMax = 1.0);

    ThresholdPredicate(const ThresholdPredicate&);

    float operator()(const Pixel &pixel) override;

private:
    const float rMin;
    const float rMax;
    const float gMin;
    const float gMax;
    const float bMin;
    const float bMax;
};


#endif //PXSORT2_THRESHOLDPREDICATE_H
