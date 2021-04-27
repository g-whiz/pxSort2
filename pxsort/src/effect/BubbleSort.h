//
// Created by gpg on 2021-04-26.
//

#ifndef PXSORT2_BUBBLESORT_H
#define PXSORT2_BUBBLESORT_H

#include "src/Effect.h"
#include "CompareAndMix.h"

namespace ps {
    class BubbleSort : public CompareAndMix {
    public:
        void attach(Segment &tile) override;

        void apply(Segment &tile) override;

        BubbleSort(const ChannelSkew &skew,
                   Segment::Traversal traversal,
                   const PixelComparator &cmp,
                   const PixelMixer &mix);

    private:
        std::unique_ptr<Effect> clone() override;
    };
}

#endif //PXSORT2_BUBBLESORT_H
