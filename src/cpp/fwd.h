#ifndef PXSORT2_COMMON_H
#define PXSORT2_COMMON_H

#include <type_traits>

namespace pxsort {
    template <typename T>
    concept Arithmetic = std::is_arithmetic<T>::value;

    class Image;

    class Sorter;

    class Skew;
    class Segment;
    class SegmentPixels;

    struct Ellipse;
    struct Polygon;

    class Map;

//    namespace fn {
//        using Modulator = std::function<float(float)>;
//
//        using CoordinateProjection = std::function<float(int32_t , int32_t)>;
//
//        using PixelProjection = std::function<float(const float *)>;
//
//
//    }
//    namespace fp {
//
//    }
}

#endif //PXSORT2_COMMON_H
