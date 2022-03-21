#ifndef PXSORT2_PARTIALBUBBLESORT_H
#define PXSORT2_PARTIALBUBBLESORT_H

#include "common.h"
#include "Sorter.h"
#include "Segment.h"

using namespace pxsort;

class pxsort::PartialBubbleSort : public Sorter {
public:
    PartialBubbleSort(const ChannelSkew &skew,
                      ::Segment::Traversal traversal,
                      PixelComparator cmp,
                      PixelMixer mix);

private:
    const PixelComparator cmp;
    const PixelMixer mix;

    void attachToSegment(Segment &tile) override;

    void applyToSegment(Segment &tile) override;

    [[nodiscard]] std::unique_ptr<Sorter> clone() const override;
};

#endif //PXSORT2_PARTIALBUBBLESORT_H
