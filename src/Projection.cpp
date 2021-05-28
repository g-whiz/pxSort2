#include "Projection.h"
#include "Predicate.h"

using namespace pxsort;

inline float clamp(float x, float lo=0.0, float hi=1.0) {
    return MIN(hi, MAX(lo, x));
}

PixelProjection projection::linear(const cv::Vec3f &M, float b) {
    PixelPredicate map = predicate::linear(M, b);
    return [=](const Pixel &p) {
        return clamp(map(p));
    };
}

PixelProjection projection::channel(Channel ch) {
    return [=](const Pixel &p) {
        return p[ch];
    };
}
