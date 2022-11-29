#ifndef PXSORT2_IMAGE_H
#define PXSORT2_IMAGE_H

#include "common.h"


class pxsort::Image {
public:
    Image() = delete;

    /** Width of this image (in pixels). */
    const int width;
    /** Height of this image (in pixels). */
    const int height;
    /** Channels in each of this Image's pixels. */
    const int depth;

    Image(const Image& other);

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     */
    Image(int width, int height, int channels);

    /**
     * Construct a new Image with uninitialized pixel pixelData.
     * WARNING: This constructor is unsafe!
     * The given pixelData pointer is assumed to point to a float array with size
     *   width * height * channels.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param channels The number of channels in each of this Image's pixels.
     * @param data The *borrowed* pixel pixelData to initialize this image with.
     * Must be of at least size (width * height * channels) with pixel pixelData
     *   arranged in row-major form.
     * i.e. Pixel (x, y) is at &pixelData[y * width * pixelDepth + x * pixelDepth], and channel
     *      i of pixel (x, y) is at &pixelData[y * width * pixelDepth + x * pixelDepth + i]
     */
    Image(int width, int height, int channels, float *data);

    /**
     * Returns a pointer to the pixel with coordinates (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    inline float *ptr(int x, int y);

    /**
     * Returns a pointer to the pixel with coordinates (x, y) in this image.
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @return A pointer to a pixel with this->pixelDepth channels.
     */
    [[nodiscard]]
    inline const float *ptr(int x, int y) const;

    /**
     * Returns the value of channel cn for the pixel at coordinates (x, y).
     * WARNING: bounds checking is only performed when compiled in debug mode.
     * @param x An integer with 0 <= x < width.
     * @param y An integer with 0 <= y < height.
     * @param cn An integer with 0 <= cn < pixelDepth.
     * @return
     */
    [[nodiscard]]
    inline float at(int x, int y, int cn) const;

private:
    const int row_stride;

    /** Array containing the underlying pixel pixelData for this Image.
     *  It has shape (width, height),
     *     with element size channels * sizeof(float)
     */
    const std::unique_ptr<float[]> data;
};

#endif //PXSORT2_IMAGE_H
