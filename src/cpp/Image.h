#ifndef PXSORT2_IMAGE_H
#define PXSORT2_IMAGE_H

#include "common.h"

#define IMAGE_MAX_WIDTH 100000
#define IMAGE_MAX_HEIGHT 100000
#define IMAGE_MAX_DEPTH 16

/**
 * An Image.
 */
class pxsort::Image {
public:
    /** Width of this image (in pixels). */
    const uint32_t width;
    /** Height of this image (in pixels). */
    const uint32_t height;
    /** Channels in each of this Image's pixels. */
    const uint32_t depth;

    Image() = delete;

    Image(const Image& other) = default;

    Image(Image&& other) = default;

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     */
    Image(uint32_t width, uint32_t height, uint32_t channels);

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * WARNING: This constructor is unsafe!
     * The given pixelData pointer is assumed to point to a float array with size
     *   width * height * channels.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     * @param src_data The *borrowed* pixel pixelData to initialize this image with.
     * Must be of at least size (width * height * channels) with pixel pixelData
     *   arranged in row-major form.
     * i.e. Pixel (x, y) is at &pixelData[y * width * pixelDepth + x * pixelDepth], and channel
     *      i of pixel (x, y) is at &pixelData[y * width * pixelDepth + x * pixelDepth + i]
     */
    Image(uint32_t width, uint32_t height, uint32_t channels, float *src_data);

    /**
     * Returns a pointer to the pixel with coordinates (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    float *ptr(uint32_t x, uint32_t y);

    /**
     * Returns a pointer to the pixel with coordinates (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    const float *ptr(uint32_t x, uint32_t y) const;

    /**
     * Returns the value of channel cn for the pixel at coordinates (x, y).
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @param cn An integer with 0 <= cn < pixelDepth.
     * @return
     */
    [[nodiscard]]
    float at(uint32_t x, uint32_t y, uint32_t cn) const;

private:
    const uint32_t row_stride;

    /** Array containing the underlying pixel pixelData for this Image.
     *  It has shape (width, height),
     *     with element size channels * sizeof(float)
     */
    const std::shared_ptr<float[]> data;
};

#endif //PXSORT2_IMAGE_H
