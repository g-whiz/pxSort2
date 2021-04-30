#include "Effect.h"
#include "Segment.h"

using namespace ps;

int Segment::getForwardIndex(int idx, SegmentTraversal t) {
    idx = MODULO(idx, this->size());
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

void Segment::setPixel(int idx, SegmentTraversal t,
                    ChannelSkew skew, const Pixel &px) {
    int fwdIdx = this->getForwardIndex(idx, t);
    this->forwardSetPixel(fwdIdx, skew, px);
}

Pixel Segment::getPixel(int idx, SegmentTraversal t, ChannelSkew skew) {
    int fwdIdx = this->getForwardIndex(idx, t);
    return this->forwardGetPixel(fwdIdx, skew);
}

int Segment::btbfToForwardIdx(int idx) {

    int depth = LOG_2(idx) - 1;
    int nSubtrees = 1 << depth;
    int subtree = MODULO((idx + 1), nSubtrees);

    int fullHeight = LOG_2(this->size()) - 1;
    int subtreeHeight = fullHeight - depth;

    int subtreeSize = (1 << subtreeHeight) - 1;
    int subtreeLoIdx = subtree * subtreeSize;

    int forwardIdx = subtreeHeight == 1 ? (2 * subtree)
                                        : (subtreeLoIdx + (subtreeSize / 2));
    return forwardIdx;
}

Segment::Segment(std::shared_ptr<Image> img)
    : img(std::move(img)), effects() {}


void Segment::attach(std::unique_ptr<Effect> e) {
    e->attach(*this);
    this->effects.push_back(std::move(e));
}

void Segment::applyEffects() {
    for (auto & effect : this->effects) {
        effect->apply(*this);
    }
}
