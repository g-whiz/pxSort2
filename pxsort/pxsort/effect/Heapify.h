//
// Created by gpg on 2021-04-26.
//

#ifndef PXSORT2_HEAPIFY_H
#define PXSORT2_HEAPIFY_H

#include <pxsort/effect/CompareAndMix.h>
#include <pxsort/Mixer.h>

namespace pxsort {
/**
 * An Effect that applies a partial pass of the heapify algorithm each time
 *   apply is called. In particular, apply performs a pseudo-bubble-down
 *   operation on some element of a binary heap.
 * Since the mix operation doesn't necessarily perform a swapper, this is not
 *   a true bubble down operation. But the effect proceeds as if it were
 *   (i.e. it assumes we are making progress towards restoring the heap
 *   property).
 */
    class Heapify : public CompareAndMix {
    public:
        void attach(Segment &tile) override;

        void apply(Segment &tile) override;

        Heapify(const ChannelSkew &skew,
                Segment::Traversal traversal,
                const PixelComparator &cmp,
                const std::function<std::pair<Pixel, Pixel>(Pixel, Pixel)> &mix);

    private:
        // Heap left and right convenience methods (for readability)
        static constexpr int left_child(int idx) { return (2 * idx) + 1; };

        static constexpr int right_child(int idx) { return (2 * idx) + 2; };

        /* Indices to keep track of our current progress in the heapify
         * operation. Each time we complete a full pass, we restart (using the
         * idx_start variable to keep track of the starting index in the heapify
         * algo). */
        int idx_start{};
        int idx{};

        std::unique_ptr<Effect> clone() override;

        std::optional<int> compareAndMix(Segment &tile,
                                         int i_parent,
                                         int child);
    };
}

#endif //PXSORT2_HEAPIFY_H
