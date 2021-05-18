#include <utility>
#include "segment/Asendorf.h"


using namespace pxsort;

int Asendorf::size() {
    return MAX(0, hiIdx - loIdx + 1);
}

Pixel Asendorf::forwardGetPixel(int idx, ChannelSkew &skew) {
    return child->getPixel(idx - loIdx, FORWARD, skew);
}

void Asendorf::forwardSetPixel(int idx, ChannelSkew &skew, const Pixel &px) {
    child->setPixel(idx, FORWARD, skew, px);
}

int Asendorf::computeLoIdx(Segment &segment,
                           const PixelPredicate &loTest) {
    int idx = 0;
    for (; idx < segment.size(); idx++){
        Pixel px = segment.getPixel(idx, FORWARD, NO_SKEW());
        if (loTest(px) >= 0)
            return idx;
    }
    return MAX(0, segment.size() - 1);
}

int Asendorf::computeHiIdx(Segment &segment, const PixelPredicate &hiTest) {
    int idx = MAX(0, segment.size() - 1);
    for (; idx >= 0; idx--) {
        Pixel px = segment.getPixel(idx, FORWARD, NO_SKEW());
        if (hiTest(px) >= 0)
            return idx;
    }
    return 0;
}

Asendorf::Asendorf(std::unique_ptr<Segment> child,
                   PixelPredicate  loTest,
                   PixelPredicate  hiTest)
    : Segment(),
      loIdx(computeLoIdx(*child, loTest)),
      hiIdx(computeHiIdx(*child, hiTest)),
      child(std::move(child)),
      loTest(std::move(loTest)),
      hiTest(std::move(hiTest)) {}
