#ifndef PXSORT2_LINEAR_MIXER_H
#define PXSORT2_LINEAR_MIXER_H

#include "common.h"
#include "PixelMixer.h"

/**
 * A LinearMixer is a PixelMixer that applies a linear transformation
 * to the input pixels. The two, 3-channel input pixels are concatenated
 * and treated as a 6-dimensional point, which is mapped to another
 * 6-dimensional point by the linear transformation.
 */
class pxsort::LinearMixer : public PixelMixer {
public:
    LinearMixer(const cv::Matx66f&);

    std::pair<Pixel, Pixel>
    operator()(const Pixel &pixel, const Pixel &pixel1) override;

private:
    cv::Matx66f T;
};


#endif //PXSORT2_LINEAR_MIXER_H
