#include "Mixer.h"
#include "opencv2/core.hpp"

using namespace pxsort;

PixelMixer mixer::linear(const cv::Matx66f &T) {
    cv::Matx66f normalized_T;
    for (int i = 0; i < 6; i++) {
        cv::normalize(T.row(i), normalized_T.row(i), 1, 0, cv::NORM_L1);
    }

    return [=](const Pixel &in1, const Pixel &in2) {
        cv::Vec6f in(in1[0], in1[1], in1[2], in2[0], in2[1], in2[2]);
        cv::Vec6f out = normalized_T * in;
        Pixel out1(out[0], out[1], out[2]);
        Pixel out2(out[3], out[4], out[5]);
        return std::pair<Pixel, Pixel>(out1, out2);
    };
}


PixelMixer mixer::swapper(mixer::Swap s) {
    switch (s) {
        case R:
            return mixer::copier(IN2_R, IN1_G, IN1_B, IN1_R, IN2_G, IN2_B);
        case G:
            return mixer::copier(IN1_R, IN2_G, IN1_B, IN2_R, IN1_G, IN2_B);
        case B:
            return mixer::copier(IN1_R, IN1_G, IN2_B, IN2_R, IN2_G, IN1_B);
        case RG:
            return mixer::copier(IN2_R, IN2_G, IN1_B, IN1_R, IN1_G, IN2_B);
        case RB:
            return mixer::copier(IN2_R, IN1_G, IN2_B, IN1_R, IN2_G, IN1_B);
        case GB:
            return mixer::copier(IN1_R, IN2_G, IN2_B, IN2_R, IN1_G, IN1_B);
        case RGB:
        default:  // assume RGB if we get an invalid Swap
            return mixer::copier(IN2_R, IN2_G, IN2_B, IN1_R, IN1_G, IN1_B);
    }
}

PixelMixer mixer::copier(mixer::InputChannel out1_r,
                         mixer::InputChannel out1_g,
                         mixer::InputChannel out1_b,
                         mixer::InputChannel out2_r,
                         mixer::InputChannel out2_g,
                         mixer::InputChannel out2_b) {
    return [=](const Pixel &in1, const Pixel &in2) {
        std::array<float, 6> in({
            in1[0], in1[1], in1[2], in2[0], in2[1], in2[2]
        });
        Pixel out1(in[out1_r], in[out1_g], in[out1_b]);
        Pixel out2(in[out2_r], in[out2_g], in[out2_b]);
        return std::pair<Pixel, Pixel>(out1, out2);
    };
}