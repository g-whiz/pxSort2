#ifndef PXSORT2_BUCKETSORT_H
#define PXSORT2_BUCKETSORT_H

#include <PixelMixer.h>
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

    BucketSort(const BucketSort&);

private:

    void attachToSegment(Segment &tile) override;

    void applyToSegment(Segment &segment) override;

    [[nodiscard]] std::unique_ptr<Sorter> clone() const override;

    int bucket(const Pixel &);

    PixelProjection project;
    PixelMixer mix;
    const int nBuckets;
};

#endif //PXSORT2_BUCKETSORT_H
