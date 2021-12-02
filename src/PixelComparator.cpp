#include "PixelComparator.h"

using namespace pxsort;

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

PixelComparator::PixelComparator(Channel ch, PixelComparator::Order order)
    : M(channel_M(ch)), order(order) {}

PixelComparator::PixelComparator(
        const cv::Vec3f &projection, PixelComparator::Order order)
    : M(projection), order(order) {}

PixelComparator::PixelComparator(const PixelComparator &other)
    : M(other.M), order(other.order) {}

inline float order_coefficient(PixelComparator::Order order) {
    switch (order) {
        case pxsort::PixelComparator::ASCENDING:
            return 1.0;
        case pxsort::PixelComparator::DESCENDING:
        default:
            return -1.0;
    }
}

float PixelComparator::operator()(const Pixel &left, const Pixel &right) {
    return order_coefficient(order) * (M.dot(right) - M.dot(left));
}

std::unique_ptr<PixelComparator> PixelComparator::clone() const {
    return std::unique_ptr<PixelComparator>(new PixelComparator(*this));
}