#include "Effect.h"

#include <utility>

using namespace ps;

Effect::Effect(const ChannelSkew &skew, Tile::Traversal traversal)
    : skew(skew), traversal(traversal){}

CompareAndMixEffect::CompareAndMixEffect(const ChannelSkew &skew,
                                         const Tile::Traversal traversal,
                                         PixelComparator cmp,
                                         PixelMixer mix)
    : cmp(std::move(cmp)), mix(std::move(mix)), Effect(skew, traversal){}

void BubbleSortEffect::attach(Tile &tile) {
    // Nothing needs to be done here.
}

void BubbleSortEffect::apply(Tile &tile) {
    for (int i = 1; i < tile.size(); i++) {
        Pixel left = tile.getPixel(i - 1, this->traversal, NO_SKEW);
        Pixel right = tile.getPixel(i, this->traversal, this->skew);

        // NOTE: this code may run very often, so it may be beneficial to
        //       performance to analyze whether the below if statement impacts
        //       branch prediction
        if (this->cmp(left, right) <= 0) {
            auto[new_left, new_right] = this->mix(left, right);
            tile.setPixel(i - 1, this->traversal, NO_SKEW, new_left);
            tile.setPixel(i, this->traversal, this->skew, new_right);
        }
    }
}

BubbleSortEffect::BubbleSortEffect(const ChannelSkew &skew,
                                   const Tile::Traversal traversal,
                                   const PixelComparator &cmp,
                                   const PixelMixer &mix)
    : CompareAndMixEffect(skew, traversal, cmp, mix) {}

std::unique_ptr<Effect> BubbleSortEffect::clone() {
    Effect *clone = new BubbleSortEffect(skew, traversal, cmp, mix);
    return std::unique_ptr<Effect>(clone);
}

void HeapifyEffect::attach(Tile &tile) {
    this->idx_start = (tile.size() / 2) - 1;
    // edge case for single-pixel tiles
    if (tile.size() == 1)
        this->idx_start = 0;

    this->idx = idx_start;
}

void HeapifyEffect::apply(Tile &tile) {
    int i = this->idx;
    int left, right;

    // Perform bubble-down pass for a single element of the heap.
    // Note: the outer if statements in the loop are just bounds checks
    //       (i.e. "Is there a left/right child?").
    do {
        left = HeapifyEffect::left_child(i);
        right = HeapifyEffect::right_child(i);

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

HeapifyEffect::HeapifyEffect(const ChannelSkew &skew,
                             const Tile::Traversal traversal,
                             const PixelComparator &cmp,
                             const PixelMixer &mix)
    : CompareAndMixEffect(skew, traversal, cmp, mix),
      idx_start(0),
      idx(0) {}


std::optional<int> HeapifyEffect::compareAndMix(Tile &tile,
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

std::unique_ptr<Effect> HeapifyEffect::clone() {
    Effect *clone = new HeapifyEffect(skew, traversal, cmp, mix);
    return std::unique_ptr<Effect>(clone);
}
