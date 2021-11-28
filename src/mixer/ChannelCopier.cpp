#include "ChannelCopier.h"

using namespace pxsort;

ChannelCopier::ChannelCopier(
        ChannelCopier::InputChannel out1_r,
        ChannelCopier::InputChannel out1_g,
        ChannelCopier::InputChannel out1_b,
        ChannelCopier::InputChannel out2_r,
        ChannelCopier::InputChannel out2_g,
        ChannelCopier::InputChannel out2_b) 
        : out1_r(out1_r), out1_g(out1_g), out1_b(out1_b),
          out2_r(out2_r), out2_g(out2_g), out2_b(out2_b){}

ChannelCopier::ChannelCopier(ChannelCopier::Swap swap)
    : ChannelCopier(IN1_R, IN1_G, IN1_B, IN2_R, IN2_G, IN2_B) {
    if (swap == R || swap == RG || swap == RB || swap == RGB) {
        out1_r = IN2_R;
        out2_r = IN1_R;
    }

    if (swap == G || swap == RG || swap == GB || swap == RGB) {
        out1_g = IN2_G;
        out2_g = IN1_G;
    }

    if (swap == B || swap == RB || swap == GB || swap == RGB) {
        out1_b = IN2_B;
        out2_b = IN1_B;
    }
}

std::pair<Pixel, Pixel>
        ChannelCopier::operator()(const Pixel &in1, const Pixel &in2) {
    std::array<float, 6> in({in1[0], in1[1], in1[2], in2[0], in2[1], in2[2]});
    Pixel out1(in[out1_r], in[out1_g], in[out1_b]);
    Pixel out2(in[out2_r], in[out2_g], in[out2_b]);
    return {out1, out2};
}

ChannelCopier::ChannelCopier(const ChannelCopier &other)
    : out1_r(other.out1_r), out1_g(other.out1_g), out1_b(other.out1_b),
      out2_r(other.out2_r), out2_g(other.out2_g), out2_b(other.out2_b) {}
