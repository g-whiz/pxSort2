#include <pxsort.h>

using namespace pxsort;

CompareAndMix::CompareAndMix(const ChannelSkew &skew,
                             const Segment::Traversal traversal,
                             PixelComparator cmp,
                             PixelMixer mix)
    : cmp(std::move(cmp)), mix(std::move(mix)), Effect(skew, traversal){}