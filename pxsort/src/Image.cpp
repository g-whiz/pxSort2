//
// Created by gpg on 2020-11-18.
//

#include "Image.h"
#include <opencv2/core/affine.hpp>

using namespace ps;
using namespace cv;

Image::Image(int width,
             int height,
             ColorSpace colorSpace,
             void *data /* borrowed */)
             : width(width), height(height),colorSpace(colorSpace) {
    // Create header for pixel data, then convert to floating point.
    const Mat rawPixels(height, width, CV_8UC4, data);
    rawPixels *= 1.f / 255;

    // Drop alpha channel, then convert color spaces as needed.
    this->pixels = Mat(height, width, CV_32FC3);
    cvtColor(rawPixels, this->pixels, COLOR_BGRA2RGB);
    if (colorSpace != RGB) {
        auto code = Image::fromRGB[colorSpace - 1];
        cvtColor(this->pixels, this->pixels, code);
    }

    normalizePixels();
}

void Image::normalizePixels() {
    Matx34f T;

    // get the "normalizing" linear transform for this Image's color space
    switch (this->colorSpace) {
        case RGB: // if RGB, we are already done
            return;

        case XYZ:
            T = Matx34f(1.05212, 0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       .918481, 0);
            break;

        case Lab:
            T = Matx34f(0.01,    0,       0,       0,
                        0,       1./254., 0,       .5,
                        0,       0,       1./254., .5);
            break;

        case Luv:
            T = Matx34f(0.01,    0,       0,       0,
                        0,       1./354., 0,       .378531,
                        0,       0,       1./262., .534351);
            break;

        case YCrCb:
            T = Matx34f(1,       0,       0,       0,
                        0,       1.00037, 0,       -.000187,
                        0,       0,       1.00059, -.000296);
            break;

        case HSV:
            T = Matx34f(1./360., 0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       1,       0);
            break;

        case HLS:
            T = Matx34f(1./360., 0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       1,       0);
            break;
    }

    // apply the transform to this Image's pixels
    transform(this->pixels, this->pixels, T);
}

void Image::denormalizePixels() {
    Matx34f T;

    // get the inverse of the "normalizing" transform
    switch (this->colorSpace) {
        case RGB: // if RGB, we are already done
            return;

        case XYZ:
            T = Matx34f(.9505,   0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       1.0888,  0);
            break;

        case Lab:
            T = Matx34f(100.,    0,       0,       0,
                        0,       254.,    0,       -174,
                        0,       0,       254.,    -174);
            break;

        case Luv:
            T = Matx34f(100.,    0,       0,       0,
                        0,       354.,    0,       -134,
                        0,       0,       262.,    -122);
            break;

        case YCrCb:
            T = Matx34f(1,       0,       0,       0,
                        0,       .999626, 0,       .000187,
                        0,       0,       .999408, .000296);
            break;

        case HSV:
            T = Matx34f(360.,    0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       1,       0);
            break;

        case HLS:
            T = Matx34f(360.,    0,       0,       0,
                        0,       1,       0,       0,
                        0,       0,       1,       0);
            break;
    }

    transform(this->pixels, this->pixels, T);
}

void * Image::to_rgb32() {
    // map pixels to original color space
    denormalizePixels();

    // allocate our output buffer & create a Mat header for it
    void * out_buf = malloc(4 * width * height);
    Mat output4c(this->height, this->width, CV_8UC4, out_buf);

    Mat output3f(this->height, this->width, CV_32FC3);
    if (this->colorSpace != RGB) {
        // convert our image back to the RGB color space if needed
        auto code = Image::toRGB[this->colorSpace - 1];
        cvtColor(this->pixels, output3f, code);
    } else {
        this->pixels.copyTo(output3f);
    }

    // convert channels from float to byte then add an alpha channel &
    // rearrange to get RGB32 in out_buf
    Mat output3c;
    output3c.convertTo(output3c, CV_8UC3, 255);
    cvtColor(output3c, output4c, COLOR_RGB2BGRA);

    // map pixels back to unit cube
    normalizePixels();

    return out_buf;
}
