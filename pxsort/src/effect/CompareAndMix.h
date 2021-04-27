//
// Created by gpg on 2021-04-26.
//

#ifndef PXSORT2_COMPAREANDMIX_H
#define PXSORT2_COMPAREANDMIX_H

#include <src/Effect.h>
#include <src/Segment.h>

namespace ps {
    /**
     * Abstract base class for effects that apply a sequence of compare-and-mix
     *   operations to a tile. For example: bubblesort-based effects.
     */
    class CompareAndMix : public Effect {
    public:
        CompareAndMix(const ChannelSkew &skew,
                      Segment::Traversal traversal,
                      PixelComparator cmp,
                      PixelMixer mix);

    protected:
        const PixelComparator cmp;
        const PixelMixer mix;

    };
}

#endif //PXSORT2_COMPAREANDMIX_H
