//
// Created by gpg on 2021-04-26.
//

#include <src/effect/Heapify.h>

using namespace ps;

void Heapify::attach(Segment &tile) {
    this->idx_start = (tile.size() / 2) - 1;
    // edge case for single-pixel segments
    if (tile.size() == 1)
        this->idx_start = 0;

    this->idx = idx_start;
}

void Heapify::apply(Segment &tile) {
    int i = this->idx;
    int left, right;

    // Perform bubble-down pass for a single element of the heap.
    // Note: the outer if statements in the loop are just bounds checks
    //       (i.e. "Is there a left/right child?").
    do {
        left = Heapify::left_child(i);
        right = Heapify::right_child(i);

        if (left < tile.size()) {
            if (auto result = this->compareAndMix(tile, i, left)) {
                i = result.value();
                continue; // swap occurred, keep bubbling
            }
        }

        if (right < tile.size()) {
            if (auto result = this->compareAndMix(tile, i, right)) {
                i = result.value();
                continue; // swap occurred, keep bubbling
            }
        }

        // no swaps occurred, this bubble-down pass is done
        break;

    } while (i < tile.size()); // bubbled to the end of our heap, we are done

    // update heapify progress, reset if we have completed a full pass of the
    //   heap
    this->idx--;
    if (this->idx < 0)
        this->idx = this->idx_start;
}

Heapify::Heapify(const ChannelSkew &skew,
                             const Segment::Traversal traversal,
                             const PixelComparator &cmp,
                             const PixelMixer &mix)
        : CompareAndMix(skew, traversal, cmp, mix),
          idx_start(0),
          idx(0) {}


std::optional<int> Heapify::compareAndMix(Segment &tile,
                                                int i_parent,
                                                int i_child) {
    Pixel parent = tile.getPixel(i_parent, this->traversal, NO_SKEW);
    Pixel child = tile.getPixel(i_child, this->traversal, this->skew);

    if (this->cmp(parent, child) < 0) {
        auto[new_parent, new_child] = this->mix(parent, child);
        tile.setPixel(i_parent, this->traversal, NO_SKEW, new_parent);
        tile.setPixel(i_child, this->traversal, this->skew, new_child);

        /* if a swap occurred, return the index of the child
         *   (to be used as the parent in the next pass) */
        return i_child;
    }

    // no swap: indicate by returning "nothing"
    return {};
}

std::unique_ptr<Effect> Heapify::clone() {
    Effect *clone = new Heapify(skew, traversal, cmp, mix);
    return std::unique_ptr<Effect>(clone);
}