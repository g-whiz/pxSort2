//
// Created by gpg on 2021-04-26.
//

#include <utility>
#include <pxsort/Effect.h>
#include <pxsort/effect/CompareAndMix.h>
#include <pxsort/Mixer.h>

using namespace pxsort;

CompareAndMix::CompareAndMix(const ChannelSkew &skew,
                             const Segment::Traversal traversal,
                             PixelComparator cmp,
                             PixelMixer mix)
    : cmp(std::move(cmp)), mix(std::move(mix)), Effect(skew, traversal){}