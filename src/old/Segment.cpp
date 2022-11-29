#include "Sorter.h"
#include "Segment.h"

using namespace pxsort;

int Segment::getForwardIndex(int idx, Traversal t) {
    idx = PXSORT_MODULO(idx, this->size());
    switch (t) {
        case REVERSE:
            return (this->size() - 1) - idx;

        case BINARY_TREE_BREADTH_FIRST:
            return btbfToForwardIdx(idx);

        case FORWARD:
        default:
            return idx;  // assume FORWARD if we get an invalid Traversal
    }
}

void Segment::setPixel(int idx, Traversal t,
                       ChannelSkew skew, const Pixel &px) {
    int fwdIdx = this->getForwardIndex(idx, t);
    this->forwardSetPixel(fwdIdx, skew, px);
}

Pixel Segment::getPixel(int idx, Traversal t, ChannelSkew skew) {
    int fwdIdx = this->getForwardIndex(idx, t);
    return this->forwardGetPixel(fwdIdx, skew);
}

int Segment::btbfToForwardIdx(int idx) {

    int depth = PXSORT_LOG_2(idx) - 1;
    int nSubtrees = 1 << depth;
    int subtree = PXSORT_MODULO((idx + 1), nSubtrees);

    int fullHeight = PXSORT_LOG_2(this->size()) - 1;
    int subtreeHeight = fullHeight - depth;

    int subtreeSize = (1 << subtreeHeight) - 1;
    int subtreeLoIdx = subtree * subtreeSize;

    int forwardIdx = subtreeHeight == 1 ? (2 * subtree)
                                        : (subtreeLoIdx + (subtreeSize / 2));
    return forwardIdx;
}

void Segment::addEffect(std::shared_ptr<Sorter> e) {
    e->attachToSegment(*this);
    this->effects.push_back(std::move(e));
}

void Segment::applyEffects() {
    for (auto const& effect : this->effects) {
        effect->applyToSegment(*this);
    }
}

std::shared_ptr<Sorter> Segment::getEffect(int i) {
    return effects.at(i);
}

class Segment::Impl {
public:
    virtual int size() = 0;

    virtual cv::Point2i pixelCoordinates(int fwdIdx) = 0;



    virtual Pixel forwardGetPixel(int idx,
                                  ChannelSkew &skew) = 0;
    virtual void forwardSetPixel(int idx,
                                 ChannelSkew &skew,
                                 const Pixel &pixel) = 0;
};

class Segment::Rectangle : Segment::Impl {
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