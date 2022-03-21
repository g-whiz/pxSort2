#include "Sorter.h"
#include "Segment.h"
#include "PixelMixer.h"
#include "PixelProjection.h"

using namespace pxsort;

Sorter::Sorter(const ChannelSkew &skew, Segment::Traversal traversal)
    : skew(skew), traversal(traversal){}


class Sorter::Impl : CloneableInterface<Impl> {
    /* Apply sorter to segment. */
    virtual Sorter::Result
    operator()(const Image &img, const Segment &seg) const = 0;
};


class Sorter::BucketSort : CloneableImpl<Sorter::Impl, Sorter::BucketSort> {
    BucketSort(const ChannelSkew &skew,
               Segment::Traversal traversal,
               const PixelProjection &project,
               const PixelMixer &mix,
               int nBuckets) {

    }
};