#ifndef PXSORT2_PARTIALBUBBLESORT_H
#define PXSORT2_PARTIALBUBBLESORT_H

#include "common.h"
#include "Effect.h"

using namespace pxsort;

class pxsort::PartialBubbleSort : public Effect {
public:
    PartialBubbleSort(const ChannelSkew &skew,
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

#endif //PXSORT2_PARTIALBUBBLESORT_H
