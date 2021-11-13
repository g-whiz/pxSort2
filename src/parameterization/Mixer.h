#ifndef PXSORT2_MIXER_H
#define PXSORT2_MIXER_H

#include "common.h"
#include "parameterization/Parameterization.h"

class pxsort::ParameterizedLinearMixer
        : pxsort::Parameterization<pxsort::PixelMixer, 6> {

    ParameterizedLinearMixer(const cv::Matx66f &baseTransform);

    std::shared_ptr<pxsort::PixelMixer>
    operator()(cv::Vec<float, 6> vec) override;

private:
    cv::Matx66f baseTransform;
};

#endif //PXSORT2_MIXER_H
