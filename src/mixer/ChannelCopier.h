#ifndef PXSORT2_CHANNELCOPIER_H
#define PXSORT2_CHANNELCOPIER_H

#include "common.h"
#include "PixelMixer.h"


/**
 * A ChannelCopier is a PixelMixer that mixes a pair of pixels by copying input
 *   channel values to output channel values according to a specified rule.
 */
class pxsort::ChannelCopier : public PixelMixer {
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
     * Create a ChannelCopier that swaps the channels of the two input pixels
     *   as specified by the given swap.
     */
    ChannelCopier(Swap);


    /**
     * Create a ChannelCopier that copies channels from the input pixels to
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
    ChannelCopier(InputChannel out1_r,
                  InputChannel out1_g,
                  InputChannel out1_b,
                  InputChannel out2_r,
                  InputChannel out2_g,
                  InputChannel out2_b);

    std::pair<Pixel, Pixel>
    operator()(const Pixel &pixel, const Pixel &pixel1) override;

private:
    InputChannel out1_r;
    InputChannel out1_g;
    InputChannel out1_b;
    InputChannel out2_r;
    InputChannel out2_g;
    InputChannel out2_b;
};


#endif //PXSORT2_CHANNELCOPIER_H
