//
// Created by gpg on 2020-11-18.
//

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
