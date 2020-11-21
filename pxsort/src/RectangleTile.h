#ifndef PXSORT2_RECTANGLETILE_H
#define PXSORT2_RECTANGLETILE_H

#include "Tile.h"

namespace ps{

    class RectangleTile : Tile {
    public:
        enum Channel {
            C0 = 0,
            C1 = 1,
            C2 = 2
        };

        int size() override;

        RectangleTile(Image * img, int width, int height, int x0, int y0);

    private:
        /** The Image containing the underlying data referenced by this tile. */
        Image * img;
        /** The width (in pixels) of this tile. */
        const int width;
        /** The height (in pixels) of this tile. */
        const int height;
        /** The x coordinate of the top-left pixel in this tile */
        const int x0;
        /** The y coordinate of the top-left pixel in this tile */
        const int y0;

        Pixel forwardGetPixel(int idx, ChannelSkew &skew) override;
        void forwardSetPixel(int idx, ChannelSkew &skew, Pixel &px) override;

        cv::Point channelCoordinates(int idx, ChannelSkew &skew,
                                 Channel channel);
    };
}


#endif //PXSORT2_RECTANGLETILE_H
