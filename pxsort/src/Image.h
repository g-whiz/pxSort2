//
// Created by gpg on 2020-11-18.
//

#ifndef PXSORT2_IMAGE_H
#define PXSORT2_IMAGE_H

#include <memory>
#include <opencv2/imgproc.hpp>

namespace ps {
    typedef cv::Vec3f Pixel;
    
    class Image {
    public:
        // TODO: - Use custom colour spaces defined by arbitrary invertible
        //           linear maps in R^3. Constructor takes a Matx33f
        //
        //       - Next-level: non-linear maps? HSV is an example. Arbitrary
        //           polar coordinate maps?
        //
        //       - Not worth implementing now. Need to see how big of a deal
        //           color spaces are when making effects.

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
         * @param data *Borrowed* pointer to RGB32 pixel data of the input
         *              image. This data is not mutated by this constructor.
         *              This data can be safely freed after this constructor
         *              returns.
         */
        Image(int width,
              int height,
              ColorSpace colorSpace,
              void * data /* borrowed */);

        /**
         * Returns a pointer to a copy of the data in this image, transformed
         *   to the RGB32 (i.e. 0xffRRGGBB) format.
         * The caller is responsible for freeing the allocated data returned by
         *  this method.
         * @return
         */
        void * to_rgb32();

    private:
        /** Mat containing the underlying pixel data for this Image.
         *  It has shape: (height, width, 3).
         *  Components are single-precision floating point numbers, regardless
         *  of color space.*/
        cv::Mat pixels;

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
}

#endif //PXSORT2_IMAGE_H
