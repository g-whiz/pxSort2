#include "Comparator.h"

using namespace pxsort;

PixelComparator comparator::channel(Channel ch,
                                    comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
         return c * (right[ch] - left[ch]);
    };
}

PixelComparator comparator::linear(const cv::Vec3f &projection,
                                   comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
        return c * (projection.dot(right) - projection.dot(left));
    };
}
