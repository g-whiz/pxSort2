#include <cassert>
#include "Image.h"

using namespace pxsort;

Image::Image(uint32_t width, uint32_t height, uint32_t channels)
: width(width), height(height),
  depth(channels), row_stride(width * channels),
  data(new float[width * height * depth]) {
    assert(width <= IMAGE_MAX_WIDTH);
    assert(height <= IMAGE_MAX_HEIGHT);
    assert(channels <= IMAGE_MAX_DEPTH);
}

Image::Image(uint32_t width, uint32_t height, uint32_t channels, float *src_data)
: Image(width, height, channels) {
    std::copy_n(src_data, width * height * channels, &this->data[0]);
}

float *Image::ptr(uint32_t x, uint32_t y) {
#ifdef PXSORT_DEBUG
    assert(0 <= x && x < width);
    assert(0 <= y && y < height);
#endif // PXSORT_DEBUG
    return &data[y * row_stride + x * depth];
}

float Image::at(uint32_t x, uint32_t y, uint32_t cn) const {
#ifdef PXSORT_DEBUG
    assert(0 <= cn && cn < depth);
#endif // PXSORT_DEBUG
    return ptr(x, y)[cn];
}

const float *Image::ptr(uint32_t x, uint32_t y) const {
    return ((Image *) this)->ptr(x, y);
}
