#include <cassert>
#include "Image.h"

using namespace pxsort;

Image::Image(int32_t width, int32_t height, int32_t channels)
: width(width), height(height),
  depth(channels), row_stride(width * channels),
  data(new float[width * height * depth]) {
    assert(0 < width && width <= IMAGE_MAX_WIDTH);
    assert(0 < height && height <= IMAGE_MAX_HEIGHT);
    assert(0 < channels && channels <= IMAGE_MAX_DEPTH);
}

Image::Image(int32_t width, int32_t height,
             int32_t channels, const std::shared_ptr<float[]>& data)
: width(width), height(height),
  depth(channels), row_stride(width * channels),
  data(data) {}

float *Image::ptr(int32_t x, int32_t y) {
#ifdef PXSORT_DEBUG
    assert(0 <= x && x < width);
    assert(0 <= y && y < height);
#endif // PXSORT_DEBUG
    /* translate y coordinate to "row" of backing array */
    const int row = (height - 1) - y;
    return &data[row * row_stride + x * depth];
}

float Image::at(int32_t x, int32_t y, int32_t cn) const {
#ifdef PXSORT_DEBUG
    assert(0 <= cn && cn < depth);
#endif // PXSORT_DEBUG
    return ptr(x, y)[cn];
}

const float *Image::ptr(int32_t x, int32_t y) const {
    return ((Image *) this)->ptr(x, y);
}
