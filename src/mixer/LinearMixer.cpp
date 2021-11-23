#include "LinearMixer.h"
#include "opencv2/core.hpp"

using namespace pxsort;

LinearMixer::LinearMixer(const cv::Matx66f &rawTransform) {
    for (int i = 0; i < 6; i++) {
        cv::normalize(rawTransform.row(i), T.row(i), 1, 0, cv::NORM_L1);
    }
}

std::pair<Pixel, Pixel>
        LinearMixer::operator()(const Pixel &in1, const Pixel &in2) {
    cv::Vec6f in(in1[0], in1[1], in1[2], in2[0], in2[1], in2[2]);
    cv::Vec6f out = T * in;
    Pixel out1(out[0], out[1], out[2]);
    Pixel out2(out[3], out[4], out[5]);
    return {out1, out2};
}
