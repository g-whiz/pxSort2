#ifndef PXSORT2_BUCKETSORT_H
#define PXSORT2_BUCKETSORT_H

#include "common.h"
#include "Sorter.h"

using namespace pxsort;

class pxsort::BucketSort : public Sorter {
public:
    BucketSort(const ChannelSkew &skew,
               SegmentTraversal traversal,
               PixelProjection project,
               PixelMixer mix,
               int nBuckets);

private:

    void attachToSegment(Segment &tile) override;

    void applyToSegment(Segment &segment) override;

    [[nodiscard]] std::unique_ptr<Sorter> clone() const override;

    PixelProjection project;
    PixelMixer mix;
    const int nBuckets;

    int bucket(const Pixel &px);

};

#endif //PXSORT2_BUCKETSORT_H
