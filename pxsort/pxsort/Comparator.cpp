//
// Created by gpg on 2021-04-29.
//

#include <pxsort/Comparator.h>

using namespace pxsort;

PixelComparator comparator::compareChannel(comparator::Channel ch,
                                           comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
         return c * (right[ch] - left[ch]);
    };
}

PixelComparator comparator::linearProjection(const Vec3f &projection,
                                             comparator::Order order) {
    float c = order == ASCENDING ? 1.0f : -1.0f;
    return [=](const Pixel &left, const Pixel &right) {
        return c * (projection.dot(right) - projection.dot(left));
    };
}
