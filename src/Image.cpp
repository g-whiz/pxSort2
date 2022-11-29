#include "Image.h"

using namespace pxsort;

Image::Image(const Image &other)
: Image(other.width, other.height, other.depth, &other.data[0]){}

Image::Image(int width, int height, int channels)
: width(width), height(height),
  depth(channels), row_stride(width * channels),
  data(new float[width * height * depth]) {}

Image::Image(int width, int height, int channels, float *src_data)
: Image(width, height, channels) {
    std::memcpy(&this->data[0], src_data,
                width * height * channels * sizeof(float));
}

float *Image::ptr(int x, int y) {
#ifdef PXSORT_DEBUG
    assert(0 <= x && x < width);
    assert(0 <= y && y < height);
#endif // PXSORT_DEBUG
    return &data[y * row_stride + x * depth];
}

float Image::at(int x, int y, int cn) const {
#ifdef PXSORT_DEBUG
    assert(0 <= cn && cn < depth);
#endif // PXSORT_DEBUG
    return ptr(x, y)[cn];
}

const float *Image::ptr(int x, int y) const {
    return ((Image *) this)->ptr(x, y);
}
