#ifndef PXSORT2_IMAGE_H
#define PXSORT2_IMAGE_H

#include "common.h"
#include <memory>
#include <opencv2/imgproc.hpp>

class ps::Image {
public:
    /** Color spaces supported by the Image class. */
    enum ColorSpace {
        RGB = 0,
        XYZ = 1,
        Lab = 2,
        Luv = 3,
        YCrCb = 4,
        HSV = 5,
        HLS = 6
    };

    /** Mat containing the underlying pixel data for this Image.
     *  It has shape: (height, width, 3).
     *  Components are single-precision floating point numbers, regardless
     *  of color space. */
    cv::Mat pixels;

    /** Width of this image (in pixels). */
    const int width;
    /** Height of this image (in pixels). */
    const int height;
    /** Color space of this image's pixels. */
    const ColorSpace colorSpace;

    /**
     * Construct a new Image using image data in the RGB32 (i.e. 0xffRRGGBB)
     *   format. Alpha channel data is unused if present.
     * @param width The width of the image (in pixels).
     * @param height The height of the image (in pixels).
     * @param colorSpace The color space to use for this image's pixels.
     * @param data Pointer to RGB32 pixel data of the input image.
     */
    Image(int width,
          int height,
          ColorSpace colorSpace,
          std::unique_ptr<uint8_t[]> data);

    /**
     * Returns a pointer to a copier of the data in this image, transformed
     *   to the RGB32 (i.e. 0xffRRGGBB) format. The width and height of the
     *   image in the returned data are the width and height of this Image.
     * @return
     */
    std::unique_ptr<uint8_t[]> to_rgb32();

private:
    /** Map pixels from their normal color space to the unit cube. */
    void normalizePixels();

    /** Map pixels from the unit cube to their normal color space. */
    void denormalizePixels();

    static constexpr cv::ColorConversionCodes fromRGB[] = {
            cv::COLOR_RGB2XYZ,
            cv::COLOR_RGB2Lab,
            cv::COLOR_RGB2Luv,
            cv::COLOR_RGB2YCrCb,
            cv::COLOR_RGB2HSV,
            cv::COLOR_RGB2HLS
    };

    static constexpr cv::ColorConversionCodes toRGB[] = {
            cv::COLOR_XYZ2RGB,
            cv::COLOR_Lab2RGB,
            cv::COLOR_Luv2RGB,
            cv::COLOR_YCrCb2RGB,
            cv::COLOR_HSV2RGB,
            cv::COLOR_HLS2RGB
    };
};

#endif //PXSORT2_IMAGE_H
