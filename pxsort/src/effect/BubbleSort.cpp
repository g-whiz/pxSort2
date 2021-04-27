//
// Created by gpg on 2021-04-26.
//

#include <src/effect/BubbleSort.h>

using namespace ps;

void BubbleSort::attach(Segment &tile) {
    // Nothing needs to be done here.
}

void BubbleSort::apply(Segment &tile) {
    for (int i = 1; i < tile.size(); i++) {
        Pixel left = tile.getPixel(i - 1, this->traversal, NO_SKEW);
        Pixel right = tile.getPixel(i, this->traversal, this->skew);

        // NOTE: this code may run very often, so it may be beneficial to
        //       performance to analyze whether the below if statement impacts
        //       branch prediction
        if (this->cmp(left, right) <= 0) {
            auto[new_left, new_right] = this->mix(left, right);
            tile.setPixel(i - 1, this->traversal, NO_SKEW, new_left);
            tile.setPixel(i, this->traversal, this->skew, new_right);
        }
    }
}

BubbleSort::BubbleSort(const ChannelSkew &skew,
                       const Segment::Traversal traversal,
                       const PixelComparator &cmp,
                       const PixelMixer &mix)
        : CompareAndMix(skew, traversal, cmp, mix) {}

std::unique_ptr<Effect> BubbleSort::clone() {
    Effect *clone = new BubbleSort(skew, traversal, cmp, mix);
    return std::unique_ptr<Effect>(clone);
}