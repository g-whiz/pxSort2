#include <cmath>
#include <numbers>
#include "Modulation.h"
#include "util.h"

namespace pxsort::modulator {
    using std::numbers::pi;

    float one(float x) {
        return 1;
    }

    Modulator identity() {
        return one;
    }

    Modulator sum(const Modulator &m1, const Modulator &m2) {
        return [=](float x) {
            return m1(x) + m2(x);
        };
    }

    Modulator phaseShift(const Modulator &mod, float phase) {
        return [=](float x) {
            return mod(x + phase);
        };
    }

    Modulator freqShift(const Modulator &mod, float factor) {
        return [=](float x) {
            return mod(x * factor);
        };
    }

    Modulator sinOscillator(float hz, float amp, float bias, float phase) {
        return [=](float x) {
            return amp * ((1 + sinf((hz * x) + phase)) / 2) + bias;
        };
    }

    Modulator triangleOscillator(float hz, float amp, float bias, float phase) {
        return [=](float x) {
            float phi = std::fmod(hz * x + phase, 2 * pi);
            phi += phi < 0 ? 2 * pi : 0;
            float const y = phi < pi ? phi / pi : 1 - (phi - pi) / pi;
            return amp * y + bias;
        };
    }

    Modulator sawOscillator(float hz, float amp, float bias, float phase) {
        return [=](float x) {
            float phi = std::fmod(hz * x + phase, 2 * pi);
            phi += phi < 0 ? 2 * pi : 0;
            float const y = phi / (2 * pi);
            return amp * y + bias;
        };
    }

    Modulator stepwise(const std::vector<float> &steps) {
        int nSteps = steps.size();
        float stepSize = pi / steps.size();

        return [=](float x) {
            int step = max(0, min(nSteps - 1, static_cast<int>(x / stepSize)));
            return steps[step];
        };
    }

    Modulator squareOscillator(float hz, float amp, float bias, float phase) {
        return [=](float x) {
            float phi = std::fmod(hz * x + phase, 2 * pi);
            phi += phi < 0 ? 2 * pi : 0;
            float const y = phi < pi ? 0 : 1;
            return amp * y + bias;
        };
    }
}