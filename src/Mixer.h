#ifndef PXSORT2_MIXER_H
#define PXSORT2_MIXER_H

#include "common.h"

namespace pxsort::mixer {
    /**
     * Create a PixelMixer that applies the given linear transformation
     * to the input pixels. The two, 3-channel input pixels are concatenated
     * and treated as a 6-dimensional point for the linear transformation.
     * @param T
     * @return
     */
    PixelMixer linear(const cv::Matx66f &T);

    /**
     * Create a PixelMixer that applies the given linear transformation
     * to the input pixels. The two, 3-channel input pixels are concatenated
     * and treated as a 6-dimensional point for the linear transformation.
     * @param T
     * @return
     */
    PixelMixer linear(const std::shared_ptr<const cv::Matx66f> &T);

    enum Swap {
        R,   // Swap the red channels.
        G,   // Swap the green channels.
        B,   // Swap the blue channels.
        RG,  // Swap the red and green channels.
        RB,  // Swap the red and blue channels.
        GB,  // Swap the green and blue channels.
        RGB  // Swap the red, green and blue channels.
    };

    /**
     * Returns a PixelMixer that swaps the channels specified by the given
     *   Swap.
     * @param s
     * @return
     */
    PixelMixer swapper(Swap s);

    enum InputChannel {
        IN1_R = 0,  // The red channel of the first input pixel.
        IN1_G = 1,  // The green channel of the first input pixel.
        IN1_B = 2,  // The blue channel of the first input pixel.
        IN2_R = 3,  // The red channel of the second input pixel.
        IN2_G = 4,  // The green channel of the second input pixel.
        IN2_B = 5   // The blue channel of the second input pixel.
    };

    /**
     * Returns a PixelMixer that copies channels from the input pixels to
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
     * @return
     */
    PixelMixer copier(InputChannel out1_r,
                      InputChannel out1_g,
                      InputChannel out1_b,
                      InputChannel out2_r,
                      InputChannel out2_g,
                      InputChannel out2_b);
}

#endif //PXSORT2_MIXER_H
