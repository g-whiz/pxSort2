#ifndef PXSORT2_PIXELMIXER_H
#define PXSORT2_PIXELMIXER_H

#include "common.h"

/**
 * A PixelMixer is a callable object that mixes/transforms two pixels to
 *   produce two new pixels.
 * In this sense, a PixelMixer can be considered an endomorphism on the set
 *   [0, 1]^6.
 */
class pxsort::PixelMixer {
public:
    enum Swap {
        R,   // Swap the red channels.
        G,   // Swap the green channels.
        B,   // Swap the blue channels.
        RG,  // Swap the red and green channels.
        RB,  // Swap the red and blue channels.
        GB,  // Swap the green and blue channels.
        RGB  // Swap the red, green and blue channels.
    };

    enum InputChannel {
        IN1_R = 0,  // The red channel of the first input pixel.
        IN1_G = 1,  // The green channel of the first input pixel.
        IN1_B = 2,  // The blue channel of the first input pixel.
        IN2_R = 3,  // The red channel of the second input pixel.
        IN2_G = 4,  // The green channel of the second input pixel.
        IN2_B = 5   // The blue channel of the second input pixel.
    };

    /**
     * Creates a PixelMixer that applies a linear transformation
     * to the input pixels. The two, 3-channel input pixels are concatenated
     * and treated as a 6-dimensional point, which is mapped to another
     * 6-dimensional point by the given linear transformation.
     */
    explicit PixelMixer(const cv::Matx66f &transform);


    /**
     * Creates a PixelMixer that swaps the channels of the two input pixels
     *   as specified by the given swap.
     */
    explicit PixelMixer(Swap swap);


    /**
     * Creates a PixelMixer that copies channels from the input pixels to
     *   the output pixels as specified.
     * @param out1_r The input channel to copy to the first output pixel's
     *               red channel.
     * @param out1_g The input channel to copy to the first output pixel's
     *               green channel.
     * @param out1_b The input channel to copy to the first output pixel's
     *               blue channel.
     * @param out2_r The input channel to copy to the second output pixel's
     *               red channel.
     * @param out2_g The input channel to copy to the second output pixel's
     *               green channel.
     * @param out2_b The input channel to copy to the second output pixel's
     *               blue channel.
     */
    PixelMixer(InputChannel out1_r,
               InputChannel out1_g,
               InputChannel out1_b,
               InputChannel out2_r,
               InputChannel out2_g,
               InputChannel out2_b);


    PixelMixer(const PixelMixer &other);

    /**
     * Applies this PixelMixer to the given pixels.
     * @return The
     */
    std::pair<Pixel, Pixel> operator()(const Pixel&, const Pixel&) const;

    std::pair<Pixel, Pixel> operator()(const std::pair<Pixel, Pixel>&) const;

    /**
     * Returns a PixelMixer that is the composition of the given PixelMixers
     *   (right-to-left associative).
     */
    PixelMixer operator<<=(const PixelMixer &other) const;

private:
    class Impl;
    class LinearImpl;
    class CopierImpl;
    class CompositionImpl;

    explicit PixelMixer(Impl *pImpl);

    const std::unique_ptr<Impl> pImpl;
};
#endif //PXSORT2_PIXELMIXER_H
