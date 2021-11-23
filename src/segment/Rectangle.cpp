#include <utility>

#include "Image.h"
#include "Rectangle.h"


using namespace pxsort;
using namespace cv;

int Rectangle::size() {
    return this->width * this->height;
}

Pixel Rectangle::forwardGetPixel(int idx, ChannelSkew &skew) {
    assert(img != nullptr);

    Point c0Coords = this->channelCoordinates(idx, skew, RED);
    float c0 = img->at(c0Coords.x, c0Coords.y, RED);

    Point c1Coords = this->channelCoordinates(idx, skew, GREEN);
    float c1 = img->at(c1Coords.x, c1Coords.y, GREEN);

    Point c2Coords = this->channelCoordinates(idx, skew, BLUE);
    float c2 = img->at(c2Coords.x, c2Coords.y, BLUE);

    return Pixel(c0, c1, c2);
}

void Rectangle::forwardSetPixel(int idx, ChannelSkew &skew,
                                const Pixel &px) {
    assert(img != nullptr);

    Point c0Coords = this->channelCoordinates(idx, skew, RED);
    *img->ptr(c0Coords.x, c0Coords.y, RED) = px[RED];

    Point c1Coords = this->channelCoordinates(idx, skew, GREEN);
    *img->ptr(c1Coords.x, c1Coords.y, GREEN) = px[GREEN];

    Point c2Coords = this->channelCoordinates(idx, skew, BLUE);
    *img->ptr(c2Coords.x, c2Coords.y, BLUE) = px[BLUE];
}

Rectangle::Rectangle(std::shared_ptr<Image> img,
                     int width, int height,
                     int x0, int y0)
    : Segment(),
      img(std::move(img)),
      width(width), height(height),
      x0(x0), y0(y0) {
    assert(width > 0);
    assert(height > 0);
    assert(width * height > 1);
    assert(0 <= x0 && x0 < this->img->width);
    assert(0 <= y0 && y0 < this->img->height);
}

Point Rectangle::channelCoordinates(int idx,
                                        ChannelSkew &skew,
                                        Channel channel) {

    int tileChX = PXSORT_MODULO(idx, this->width);
    int tileChY = PXSORT_MODULO(idx / this->width, this->height);

    int dx = skew(0, channel);
    int dy = skew(1, channel);

    int chX = PXSORT_MODULO(PXSORT_MODULO(tileChX + dx, this->width) + this->x0,
                            img->width);
    int chY = PXSORT_MODULO(PXSORT_MODULO(tileChY + dy, this->height) + this->y0,
                            img->height);

    return {chX, chY};
}
