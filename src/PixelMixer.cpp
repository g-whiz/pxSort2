#include "PixelMixer.h"
#include "opencv2/core.hpp"

using namespace pxsort;


class PixelMixer::Impl : public CloneableInterface<Impl> {
public:
    virtual std::pair<Pixel, Pixel>
    operator()(const Pixel&, const Pixel&) const = 0;
};

class PixelMixer::LinearImpl
        : public CloneableImpl<Impl, LinearImpl> {
    const cv::Matx66f T;

public:
    LinearImpl(const cv::Matx66f &T) : T(normalized(T)) {}

    LinearImpl(const LinearImpl &other) : T(other.T) {}

private:
    std::pair<Pixel, Pixel>
    operator()(const Pixel &in1, const Pixel &in2) const override {
        cv::Vec6f in(in1[0], in1[1], in1[2], in2[0], in2[1], in2[2]);
        cv::Vec6f out = T * in;
        Pixel out1(out[0], out[1], out[2]);
        Pixel out2(out[3], out[4], out[5]);
        return {out1, out2};
    }

    static cv::Matx66f normalized(const cv::Matx66f &rawT) {
        cv::Matx66f T;
        for (int i = 0; i < 6; i++) {
            cv::normalize(rawT.row(i), T.row(i), 1, 0, cv::NORM_L1);
        }
        return T;
    }
};

class PixelMixer::CopierImpl
        : public CloneableImpl<Impl, CopierImpl> {
    InputChannel out1_r;
    InputChannel out1_g;
    InputChannel out1_b;
    InputChannel out2_r;
    InputChannel out2_g;
    InputChannel out2_b;

public:
    explicit CopierImpl(Swap swap)
    : CopierImpl(IN1_R, IN1_G, IN1_B, IN2_R, IN2_G, IN2_B) {
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

    CopierImpl(InputChannel out1_r,
               InputChannel out1_g,
               InputChannel out1_b,
               InputChannel out2_r,
               InputChannel out2_g,
               InputChannel out2_b)
               : out1_r(out1_r), out1_g(out1_g), out1_b(out1_b),
                 out2_r(out2_r), out2_g(out2_g), out2_b(out2_b) {}

    CopierImpl(const CopierImpl &other)
    : out1_r(other.out1_r), out1_g(other.out1_g), out1_b(other.out1_b),
      out2_r(other.out2_r), out2_g(other.out2_g), out2_b(other.out2_b) {}

private:
    std::pair<Pixel, Pixel>
    operator()(const Pixel &in1, const Pixel &in2) const override {
        std::array<float, 6> in({in1[0], in1[1], in1[2],
                                 in2[0], in2[1], in2[2]});
        Pixel out1(in[out1_r], in[out1_g], in[out1_b]);
        Pixel out2(in[out2_r], in[out2_g], in[out2_b]);
        return {out1, out2};
    }
};

class PixelMixer::CompositionImpl
        : public CloneableImpl<Impl, CompositionImpl> {
    PixelMixer f;
    PixelMixer g;

public:
    CompositionImpl(const PixelMixer &f, const PixelMixer &g) : f(f), g(g) {}

    CompositionImpl(const CompositionImpl &other) : f(other.f), g(other.g) {}

private:
    std::pair<Pixel, Pixel>
    operator()(const Pixel &in1, const Pixel &in2) const override {
        return f(g(in1, in2));
    }
};

pxsort::PixelMixer::PixelMixer(const cv::Matx66f &transform)
    : pImpl(new LinearImpl(transform)){}

pxsort::PixelMixer::PixelMixer(PixelMixer::Swap swap)
    : pImpl(new CopierImpl(swap)) {}

pxsort::PixelMixer::PixelMixer(PixelMixer::InputChannel out1_r,
                               PixelMixer::InputChannel out1_g,
                               PixelMixer::InputChannel out1_b,
                               PixelMixer::InputChannel out2_r,
                               PixelMixer::InputChannel out2_g,
                               PixelMixer::InputChannel out2_b)
    : pImpl(new CopierImpl(out1_r, out1_g, out1_b, out2_r, out2_g, out2_b)) {}

pxsort::PixelMixer::PixelMixer(const PixelMixer &other)
    : pImpl(other.pImpl->clone()) {}

std::pair<Pixel, Pixel>
pxsort::PixelMixer::operator()(const Pixel &in1, const Pixel &in2) const {
    return (*pImpl)(in1, in2);
}

std::pair<Pixel, Pixel>
pxsort::PixelMixer::operator()(const std::pair<Pixel, Pixel> &in) const {
    return (*pImpl)(in.first, in.second);
}

PixelMixer pxsort::PixelMixer::operator<<=(const PixelMixer &that) const {
    return PixelMixer(new CompositionImpl(*this, that));
}

pxsort::PixelMixer::PixelMixer(PixelMixer::Impl *pImpl) : pImpl(pImpl) {}
