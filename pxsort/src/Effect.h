#ifndef PXSORT2_EFFECT_H
#define PXSORT2_EFFECT_H

#include "Tile.h"

namespace ps {

    /**
     * A PixelComparator is a callable object that imposes some ordering on
     *   Pixels.
     * The value returned by a PixelComparator is negative, zero, or positive
     *   when the first pixel is less-than, equal-to, or greater-than the second
     *   pixel, respectively.
     */
    typedef std::function<int(Pixel, Pixel)> PixelComparator;

    /**
     * A PixelMixer is a callable object that mixes/transforms two pixels to
     *   produce two new pixels.
     * In this sense, a PixelMixer can be considered an endomorphism on the set
     *   [0, 1]^6.
     */
    typedef std::function<std::pair<Pixel, Pixel>(Pixel, Pixel)> PixelMixer;

    /**
     * Base interface for effects. An Effect is associated with a specific Tile.
     * Each time that apply() is called, an Effect will apply its effect to the
     *   given Tile, mutating the underlying Image in the process.
     *
     * Although both attach and apply take a Tile as a parameter, the same Tile
     *   that was attached to an effect must be given to apply. Not doing so
     *   may result in unspecified behaviour.
     * In practice, apply should never be called explicitly by the library user.
     * Instead, it is called by a Tile through a double-dispatch mechanism.
     */
    class Effect {
    public:
        /**
         * Associates this effect with the given Tile. This method should be
         * called once at initialization.
         *
         * The purpose of this method is to initialize any internal state in an
         *   effect using the information about the given Tile (i.e. its size).
         * @param tile
         */
        virtual void attach(Tile &tile) = 0;

        /**
         * Applies this effect to a Tile
         * @param tile
         */
        virtual void apply(Tile &tile) = 0;

        /**
         * Returns a copy of this effect.
         * @return
         */
        virtual std::unique_ptr<Effect> clone() = 0;

        Effect(const ChannelSkew &skew,
               const Tile::Traversal traversal);

    protected:
        const ChannelSkew skew;
        const Tile::Traversal traversal;
    };


    /**
     * Abstract base class for effects that apply a sequence of compare-and-mix
     *   operations to a tile. For example: bubblesort-based effects.
     */
    class CompareAndMixEffect : public Effect {
    public:
        CompareAndMixEffect(const ChannelSkew &skew,
                            const Tile::Traversal traversal,
                            PixelComparator cmp,
                            PixelMixer mix);

    protected:
        const PixelComparator cmp;
        const PixelMixer mix;

    };


    class BubbleSortEffect : CompareAndMixEffect {
    public:
        void attach(Tile &tile) override;
        void apply(Tile &tile) override;
        BubbleSortEffect(const ChannelSkew &skew,
                         const Tile::Traversal traversal,
                         const PixelComparator &cmp,
                         const PixelMixer &mix);

    private:
        std::unique_ptr<Effect> clone() override;
    };

    /**
     * An Effect that applies a partial pass of the heapify algorithm each time
     *   apply is called. In particular, apply performs a pseudo-bubble-down
     *   operation on some element of a binary heap.
     * Since the mix operation doesn't necessarily perform a swap, this is not
     *   a true bubble down operation. But the effect proceeds as if it were
     *   (i.e. it assumes we are making progress towards restoring the heap
     *   property).
     */
    class HeapifyEffect : CompareAndMixEffect {
    public:
        void attach(Tile &tile) override;
        void apply(Tile &tile) override;
        HeapifyEffect(const ChannelSkew &skew,
                      const Tile::Traversal traversal,
                      const PixelComparator &cmp,
                      const PixelMixer &mix);

    private:
        // Heap left and right convenience methods (for readability)
        static constexpr int left_child(int idx) {return (2 * idx) + 1;};
        static constexpr int right_child(int idx) {return (2 * idx) + 2;};

        /* Indices to keep track of our current progress in the heapify
         * operation. Each time we complete a full pass, we restart (using the
         * idx_start variable to keep track of the starting index in the heapify
         * algo). */
        int idx_start{};
        int idx{};

        std::unique_ptr<Effect> clone() override;

        std::optional<int> compareAndMix(Tile &tile, int i_parent, int child);
    };
}




#endif //PXSORT2_EFFECT_H
