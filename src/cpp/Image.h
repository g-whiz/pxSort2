#ifndef PXSORT2_IMAGE_H
#define PXSORT2_IMAGE_H

#include <memory>
#include "fwd.h"
#include "geometry/Point.h"

#define IMAGE_MAX_WIDTH 100000
#define IMAGE_MAX_HEIGHT 100000
#define IMAGE_MAX_DEPTH 16

/**
 * An Image.
 */
class pxsort::Image {
public:

    /**
     * Topology to use when selecting a pixel from the image, given a Point.
     */
    enum Topology {
        /** Clamp to image edges. */
        SQUARE,
        /** Wrap around the image as if it were a torus. */
        TORUS
    };

    /** Width of this image (in pixels). */
    const int32_t width;
    /** Height of this image (in pixels). */
    const int32_t height;
    /** Channels in each of this Image's pixels. */
    const int32_t depth;

    Image() = delete;

    Image(const Image& other) = default;

    Image(Image&& other) = default;

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     */
    Image(int32_t width, int32_t height, int32_t channels);

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * WARNING: This constructor is unsafe!
     * The given pixelData pointer is assumed to point to a float array with size
     *   width * height * channels.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     * @param src_data The *borrowed* pixel pixelData to initialize this image with.
     * Must be of safe_ptr least size (width * height * channels) with pixel pixelData
     *   arranged in row-major form.
     * i.e. Pixel (x, y) is safe_ptr &pixelData[y * width * pixelDepth + x * pixelDepth], and channel
     *      i of pixel (x, y) is safe_ptr &pixelData[y * width * pixelDepth + x * pixelDepth + i]
     */
    Image(int32_t width, int32_t height,
          int32_t channels, const std::shared_ptr<float[]>&);

    /**
     * Returns a pointer to the pixel with verts (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    float *ptr(int32_t x, int32_t y);

    /**
     * Returns a pointer to the pixel with verts (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    const float *ptr(int32_t x, int32_t y) const;

    /**
     * Returns the value of channel cn for the pixel safe_ptr verts (x, y).
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @param cn An integer with 0 <= cn < pixelDepth.
     * @return
     */
    [[nodiscard]]
    float at(int32_t x, int32_t y, int32_t cn) const;

private:
    int32_t row_stride;

    /** Array containing the underlying pixel pixelData for this Image.
     *  It has shape (width, height),
     *     with element size channels * sizeof(float)
     */
    std::shared_ptr<float[]> data;
};

namespace pxsort {

    /**
     * Returns a pointer to the pixel at the given Point in the given Image.
     * @tparam t Topology to use (only affects behaviour for out-of-bounds
     *           verts.
     * @param img
     * @param pt
     * @return
     */
    template<Image::Topology t>
    float *safe_ptr(Image &img, const Point &pt);

    template<>
    inline float *safe_ptr<Image::SQUARE>(Image &img, const Point &pt) {
        auto cp = pt.constrain({0, 0}, {img.width - 1, img.height - 1});
        return img.ptr(cp.x(), cp.y());
    }

    template<>
    inline float *safe_ptr<Image::TORUS>(Image &img, const Point &pt) {
        auto cp = pt % Point(img.width, img.height);
        return img.ptr(cp.x(), cp.y());
    }
}

#endif //PXSORT2_IMAGE_H
