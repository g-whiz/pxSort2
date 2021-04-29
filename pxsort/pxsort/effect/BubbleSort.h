//
// Created by gpg on 2021-04-26.
//

#ifndef PXSORT2_BUBBLESORT_H
#define PXSORT2_BUBBLESORT_H

#include <pxsort/Effect.h>
#include <pxsort/effect/CompareAndMix.h>
#include <pxsort/Mixer.h>

namespace pxsort {
    class BubbleSort : public CompareAndMix {
    public:
        void attach(Segment &tile) override;

        void apply(Segment &tile) override;

        BubbleSort(const ChannelSkew &skew,
                   Segment::Traversal traversal,
                   const PixelComparator &cmp,
                   const std::function<std::pair<Pixel, Pixel>(Pixel, Pixel)> &mix);

    private:
        std::unique_ptr<Effect> clone() override;
    };
}

#endif //PXSORT2_BUBBLESORT_H
