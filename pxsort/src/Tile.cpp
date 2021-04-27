#include "Tile.h"

using namespace ps;

int Tile::getForwardIndex(int idx, ps::Tile::Traversal t) {
    idx = MODULO(idx, this->size());
    switch (t) {
        case FORWARD:
            return idx;

        case REVERSE:
            return (this->size() - 1) - idx;

        case BINARY_TREE_BREADTH_FIRST:
            return btbfToForwardIdx(idx);
    }
}

void Tile::setPixel(int idx, Tile::Traversal t,
                    ChannelSkew skew, const Pixel &px) {
    int fwdIdx = this->getForwardIndex(idx, t);
    this->forwardSetPixel(fwdIdx, skew, px);
}

Pixel Tile::getPixel(int idx, Tile::Traversal t, ChannelSkew skew) {
    int fwdIdx = this->getForwardIndex(idx, t);
    return this->forwardGetPixel(fwdIdx, skew);
}

int Tile::btbfToForwardIdx(int idx) {

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

Tile::Tile(std::weak_ptr<Image> &img)
    : weakImg(img), img(), effects() {}

bool Tile::acquireImg() {
    if (this->weakImg.expired())
        return false;

    this->img = this->weakImg.lock();
    return true;
}

void Tile::releaseImg() {
    this->img.reset();
}

void Tile::attach(std::unique_ptr<Effect> e) {
    this->effects.push_back(std::move(e));
    e->attach(*this);
}

void Tile::applyEffects() {
    assert(this->img == nullptr);

    // Do nothing if the underlying image has been deallocated.
    // TODO: Debug log message here.
    if (!acquireImg())
        return;

    for (auto & effect : this->effects) {
        effect->apply(*this);
    }
    releaseImg();
}
