//
// Created by gpg on 2021-04-26.
//

#include <utility>
#include <src/Effect.h>
#include <src/effect/CompareAndMix.h>

using namespace ps;

CompareAndMix::CompareAndMix(const ps::ChannelSkew &skew,
                                         const ps::Segment::Traversal traversal,
                                         ps::PixelComparator cmp,
                                         ps::PixelMixer mix)
    : cmp(std::move(cmp)), mix(std::move(mix)), ps::Effect(skew, traversal){}