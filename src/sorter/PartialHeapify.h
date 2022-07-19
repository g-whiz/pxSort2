#ifndef PXSORT2_PARTIALHEAPIFY_H
#define PXSORT2_PARTIALHEAPIFY_H

#include "common.h"
#include "Sorter.h"
#include "Segment.h"

/**
 * An Sorter that applies a partial pass of the heapify algorithm each time
 *   apply is called. In particular, applyToSegment performs a pseudo-bubble-down
 *   operation on some element of a binary heap.
 * Since the mix operation doesn't necessarily perform a swap, this is not
 *   a true bubble down operation. But the sorter proceeds as if it were
 *   (i.e. it assumes we are making progress towards restoring the heap
 *   property).
 */
class pxsort::PartialHeapify : public Sorter {
public:
    void attachToSegment(Segment &tile) override;

    void applyToSegment(Segment &tile) override;

    PartialHeapify(const ChannelSkew &skew,
            ::Segment::Traversal traversal,
            PixelComparator cmp,
            PixelMixer mix);

private:
    const PixelComparator cmp;
    const PixelMixer mix;

    // Heap left and right convenience methods (for readability)
    static constexpr int left_child(int idx) { return (2 * idx) + 1; };

    static constexpr int right_child(int idx) { return (2 * idx) + 2; };

    /* Indices to keep track of our current progress in the heapify
     * operation. Each time we complete a full pass, we restart (using the
     * idx_start variable to keep track of the starting index in the heapify
     * algo). */
    int idx_start{};
    int idx{};

    [[nodiscard]] std::unique_ptr<Sorter> clone() const override;

    std::optional<int> compareAndMix(Segment &tile,
                                     int i_parent,
                                     int child);
};


#endif //PXSORT2_PARTIALHEAPIFY_H
