#include "PixelProjection.h"

#include <memory>

using namespace pxsort;

PixelProjection::PixelProjection(const cv::Vec3f &M, float b) : M(M), b(b) {}

static inline cv::Vec3f channel_M(Channel channel) {
    switch (channel) {
        case RED:
            return {1.0, 0.0, 0.0};
        case GREEN:
            return {0.0, 1.0, 0.0};
        case BLUE:
        default:
            return {0.0, 0.0, 1.0};
    }
}

PixelProjection::PixelProjection(Channel channel)
    : M(channel_M(channel)), b(0) {}

PixelProjection::PixelProjection(const PixelProjection &other)
    : M(other.M), b(other.b) {}

float PixelProjection::operator()(const Pixel &pixel) {
    return clamp<float>(M.dot(pixel) + b, 0, 1);
}

std::unique_ptr<PixelProjection> PixelProjection::clone() const {
    return std::make_unique<PixelProjection>(*this);
}