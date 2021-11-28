#include "sorter/BucketSort.h"
#include "Segment.h"

#include <utility>

using namespace pxsort;

void BucketSort::attachToSegment(Segment &tile) {}

void BucketSort::applyToSegment(Segment &segment) {
    // count pixels in each bucket
    std::vector<int> counts(nBuckets, 0);
    for (int i = 0; i < segment.size(); i++) {
        int bkt_idx = bucket(segment.getPixel(i, traversal, skew));
        counts[bkt_idx]++;
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
        int index = indices.at(b);

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

std::unique_ptr<Sorter> BucketSort::clone() const {
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
    : Sorter(skew, traversal),
      project(std::move(project)),
      mix(std::move(mix)),
      nBuckets(nBuckets) {}

/* Determines which bucket a pixel belongs to.
 * Uses a binary search to remain efficient with large numbers of buckets. */
int BucketSort::bucket(const Pixel &px) {
    double step = 1.0 / (double) nBuckets;
    auto px_proj = clamp<float>(project(px), 0.0, 1.0);

    int b = clamp<int>(int (px_proj / step), 0, nBuckets - 1);
    return b;
}

BucketSort::BucketSort(const BucketSort &other)
: BucketSort(other.skew, other.traversal, other.project,
             other.mix, other.nBuckets) {}
