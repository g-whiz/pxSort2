#ifndef PXSORT2_BUBBLESORT_H
#define PXSORT2_BUBBLESORT_H

#include <pxsort/common.h>
#include "Effect.h"

using namespace ps;

class ps::BubbleSort : public Effect {
public:
    BubbleSort(const ChannelSkew &skew,
               SegmentTraversal traversal,
               PixelComparator cmp,
               PixelMixer mix);

private:
    const PixelComparator cmp;
    const PixelMixer mix;

    void attach(Segment &tile) override;

    void apply(Segment &tile) override;

    std::unique_ptr<Effect> clone() override;
};

#endif //PXSORT2_BUBBLESORT_H
