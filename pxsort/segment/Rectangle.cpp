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

    Point c0Coords = this->channelCoordinates(idx, skew, R);
    float c0 = img->pixels.at<Vec3f>(c0Coords.y, c0Coords.x)[R];

    Point c1Coords = this->channelCoordinates(idx, skew, G);
    float c1 = img->pixels.at<Vec3f>(c1Coords.y, c1Coords.x)[G];

    Point c2Coords = this->channelCoordinates(idx, skew, B);
    float c2 = img->pixels.at<Vec3f>(c2Coords.y, c2Coords.x)[B];

    return Pixel(c0, c1, c2);
}

void Rectangle::forwardSetPixel(int idx, ChannelSkew &skew,
                                    const Pixel &px) {
    assert(img != nullptr);

    Point c0Coords = this->channelCoordinates(idx, skew, R);
    (*img->pixels.ptr<Vec3f>(c0Coords.y, c0Coords.x))[R] = px[R];

    Point c1Coords = this->channelCoordinates(idx, skew, G);
    (*img->pixels.ptr<Vec3f>(c1Coords.y, c1Coords.x))[G] = px[G];

    Point c2Coords = this->channelCoordinates(idx, skew, B);
    (*img->pixels.ptr<Vec3f>(c2Coords.y, c2Coords.x))[B] = px[B];
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

    int tileChX = PS_MODULO(idx, this->width);
    int tileChY = PS_MODULO(idx / this->width, this->height);

    int dx = skew(0, channel);
    int dy = skew(1, channel);

    int chX = PS_MODULO(PS_MODULO(tileChX + dx, this->width) + this->x0,
                        img->width);
    int chY = PS_MODULO(PS_MODULO(tileChY + dy, this->height) + this->y0,
                        img->height);

    return {chX, chY};
}
