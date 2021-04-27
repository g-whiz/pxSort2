#include "Rectangle.h"

using namespace ps;
using namespace cv;

int Rectangle::size() {
    return this->width * this->height;
}

Pixel Rectangle::forwardGetPixel(int idx, ChannelSkew &skew) {
    assert(img != nullptr);

    Point c0Coords = this->channelCoordinates(idx, skew, C0);
    float c0 = img->pixels.at<float>(c0Coords.x, c0Coords.y, C0);

    Point c1Coords = this->channelCoordinates(idx, skew, C1);
    float c1 = img->pixels.at<float>(c1Coords.x, c1Coords.y, C1);

    Point c2Coords = this->channelCoordinates(idx, skew, C2);
    float c2 = img->pixels.at<float>(c2Coords.x, c2Coords.y, C2);

    return Pixel(c0, c1, c2);
}

void Rectangle::forwardSetPixel(int idx, ChannelSkew &skew,
                                    const Pixel &px) {
    assert(img != nullptr);

    Point c0Coords = this->channelCoordinates(idx, skew, C0);
    (*img->pixels.ptr<float>(c0Coords.x, c0Coords.y, C0)) = px[C0];

    Point c1Coords = this->channelCoordinates(idx, skew, C1);
    (*img->pixels.ptr<float>(c1Coords.x, c1Coords.y, C1)) = px[C1];

    Point c2Coords = this->channelCoordinates(idx, skew, C2);
    (*img->pixels.ptr<float>(c2Coords.x, c2Coords.y, C2)) = px[C2];
}

Rectangle::Rectangle(std::weak_ptr<Image> &img,
                             int width, int height,
                             int x0, int y0)
    : Segment(img), width(width), height(height), x0(x0), y0(y0) {
    assert(width > 0);
    assert(height > 0);
    assert(width * height > 1);
}

Point Rectangle::channelCoordinates(int idx,
                                        ChannelSkew &skew,
                                        Channel channel) {
    int image_width;

    // Retrieve width of the underlying image.
    // Assume width is 1 if the weak_ptr to image is expired.
    if (!acquireImg())
        image_width = 1;
    else
        image_width = img->width;
    releaseImg();

    int tileChX = MODULO(idx, this->width);
    int tileChY = MODULO(idx / this->width, this->height);

    int dx = skew(0, channel);
    int dy = skew(1, channel);

    int chX = MODULO(MODULO(tileChX + dx, this->width) + this->x0,
                     image_width);
    int chY = MODULO(MODULO(tileChY + dy, this->height) + this->y0,
                     this->height);

    return {chX, chY};
}
