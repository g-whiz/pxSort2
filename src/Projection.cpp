#include "Projection.h"
#include "Predicate.h"

using namespace pxsort;

PixelProjection projection::linear(const cv::Vec3f &M, float b) {
    PixelPredicate map = predicate::linear(M, b);
    return [=](const Pixel &p) {
        return clamp<float>(map(p), 0.0, 1.0);
    };
}

PixelProjection projection::channel(Channel ch) {
    return [=](const Pixel &p) {
        return p[ch];
    };
}
