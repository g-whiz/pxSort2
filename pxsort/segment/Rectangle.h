#ifndef PXSORT2_RECTANGLE_H
#define PXSORT2_RECTANGLE_H

#include "Segment.h"

class pxsort::Rectangle : public Segment {
public:
    enum Channel {
        R = 0,
        G = 1,
        B = 2
    };

    int size() override;

    Rectangle(std::shared_ptr<Image> img,
              int width, int height,
              int x0, int y0);

protected:
    Pixel forwardGetPixel(int idx,
                          ChannelSkew &skew) override;
    void forwardSetPixel(int idx,
                         ChannelSkew &skew,
                         const Pixel &px) override;

private:
    /**
     * Pointer to the underlying Image data for this Rectangle.
     */
    std::shared_ptr<Image> img;

    /** The width (in pixels) of this tile. */
    const int width;
    /** The height (in pixels) of this tile. */
    const int height;
    /** The x coordinate of the top-left pixel in this tile */
    const int x0;
    /** The y coordinate of the top-left pixel in this tile */
    const int y0;

    cv::Point channelCoordinates(int idx, ChannelSkew &skew, Channel channel);
};

#endif //PXSORT2_RECTANGLE_H
