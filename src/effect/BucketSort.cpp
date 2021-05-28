#include "effect/BucketSort.h"
#include "Segment.h"

#include <utility>

using namespace pxsort;

void BucketSort::attachToSegment(Segment &tile) {}

void BucketSort::applyToSegment(Segment &segment) {
    // count pixels in each bucket
    std::vector<int> counts(nBuckets, 0);
    for (int i = 0; i < segment.size(); i++) {
        int b = bucket(segment.getPixel(i, traversal, skew));
        counts[b]++;
    }

    // compute the starting indices of each bucket
    std::vector<int> indices(nBuckets, 0);
    for (int b = 1; b < nBuckets; b++) {
        indices[b] = counts[b-1] + indices[b - 1];
    }

    // sort pixels
    std::vector<Pixel> sorted(segment.size());
    for (int i = 0; i < segment.size(); i++) {
        Pixel px = segment.getPixel(i, traversal, skew);
        int b = bucket(px);
        int index = indices[b];

        sorted[index] = px;
        indices[b]++;
    }

    // mix sorted pixels back into segment
    for (int i = 0; i < segment.size(); i++) {
        Pixel original = segment.getPixel(i, traversal, skew);
        auto [result, _] = mix(original, sorted[i]);
        segment.setPixel(i, traversal, skew, result);
    }
}

std::unique_ptr<Effect> BucketSort::clone() const {
    return std::make_unique<BucketSort>(skew,
                                        traversal,
                                        project,
                                        mix,
                                        nBuckets);
}

std::vector<cv::Vec2d> makeBucketRanges(int nBuckets,
                                        double lo = 0.0,
                                        double hi = 1.0) {
    double step = (hi - lo) / nBuckets;
    std::vector<cv::Vec2d> buckets;

    cv::Vec2d range(lo, step);
    for (int i = 0; i < nBuckets; i++) {
        buckets.push_back(range);
        range = range + cv::Vec2d(step, step);
    }
    // set ends of first & last buckets' ranges to negative & positive infinity
    // to account for any overflows
    buckets[0][0] = -HUGE_VAL;
    buckets[nBuckets - 1][1] = HUGE_VAL;
    return buckets;
}

BucketSort::BucketSort(const ChannelSkew &skew,
                       SegmentTraversal traversal,
                       PixelProjection project,
                       PixelMixer mix,
                       int nBuckets)
    : Effect(skew, traversal),
      project(std::move(project)),
      mix(std::move(mix)),
      nBuckets(nBuckets),
      bucketRanges(makeBucketRanges(nBuckets)){}

/* Determines which bucket a pixel belongs to.
 * Uses a binary search to remain efficient with large numbers of buckets. */
int BucketSort::bucket(const Pixel &px) {
    // the image of px under projection 'project'
    float image = project(px);

    int h = PS_LOG_2(nBuckets);
    int b = (1 << h) - 1;
    while (true) {
        h--;
        if (image < bucketRanges[b][0])
            b = b - (1 << h);
        else if (image > bucketRanges[b][1])
            b = b + (1 << h);
        else break;
    }

    return b;
}
