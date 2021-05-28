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

    void attachToSegment(Segment &tile) override;

    void applyToSegment(Segment &tile) override;

    [[nodiscard]] std::unique_ptr<Effect> clone() const override;
};

#endif //PXSORT2_PARTIALBUBBLESORT_H
