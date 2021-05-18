#ifndef PXSORT2_BUCKETSORT_H
#define PXSORT2_BUCKETSORT_H

#include "common.h"
#include "Effect.h"

class pxsort::BucketSort : public Effect {
public:
    BucketSort(const ChannelSkew &skew,
               SegmentTraversal traversal,
               PixelProjection project,
               PixelMixer mix,
               int nBuckets);

    void attach(Segment &tile) override;

    void apply(Segment &segment) override;

    std::unique_ptr<Effect> clone() override;

private:
    PixelProjection project;
    PixelMixer mix;
    const int nBuckets;
    std::vector<cv::Vec2d> bucketRanges;

    int bucket(const Pixel &px);

};

#endif //PXSORT2_BUCKETSORT_H
