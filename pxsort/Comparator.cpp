#include "Comparator.h"

using namespace ps;

PixelComparator comparator::compareChannel(comparator::Channel ch,
                                           comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
         return c * (right[ch] - left[ch]);
    };
}

PixelComparator comparator::linearProjection(const cv::Vec3f &projection,
                                             comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
        return c * (projection.dot(right) - projection.dot(left));
    };
}
