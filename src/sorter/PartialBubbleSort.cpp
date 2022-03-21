#include <utility>
#include "PartialBubbleSort.h"
#include "Segment.h"

using namespace pxsort;

void PartialBubbleSort::attachToSegment(Segment &tile) {
    // Nothing needs to be done here.
}

void PartialBubbleSort::applyToSegment(Segment &tile) {
    for (int i = 1; i < tile.size(); i++) {
        Pixel left = tile.getPixel(i - 1, this->traversal, NO_SKEW());
        Pixel right = tile.getPixel(i, this->traversal, this->skew);

        // NOTE: this code may run very often, so it may be beneficial to
        //       performance to analyze whether the below if statement impacts
        //       branch prediction
        if (this->cmp(left, right) <= 0) {
            auto[new_left, new_right] = this->mix(left, right);
            tile.setPixel(i - 1, this->traversal, NO_SKEW(), new_left);
            tile.setPixel(i, this->traversal, this->skew, new_right);
        }
    }
}

PartialBubbleSort::PartialBubbleSort(const ChannelSkew &skew,
                                     const ::Segment::Traversal traversal,
                                     PixelComparator cmp,
                                     PixelMixer mix)
        : Sorter(skew, traversal), cmp(std::move(cmp)), mix(std::move(mix)) {}

std::unique_ptr<Sorter> PartialBubbleSort::clone() const {
    return std::make_unique<PartialBubbleSort>(skew, traversal, cmp, mix);
}