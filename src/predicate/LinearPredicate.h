//
// Created by gpg on 2021-11-24.
//

#ifndef PXSORT2_LINEARPREDICATE_H
#define PXSORT2_LINEARPREDICATE_H

#include "common.h"
#include "PixelPredicate.h"

/**
 * A LinearPredicate is a PixelPredicate derived from the linear transformation:
 *     f(p) = M*p + b
 * @param M A linear map from [0, 1]^3 to RED in matrix form.
 * @param b A bias parameter
 */
class pxsort::LinearPredicate :
        public CloneableImpl<PixelPredicate, LinearPredicate> {
public:
    float operator()(const Pixel &pixel) override;

    LinearPredicate(const cv::Vec3f&, float);

    LinearPredicate(const LinearPredicate&);

private:
    const cv::Vec3f M;
    const float b;
};


#endif //PXSORT2_LINEARPREDICATE_H
