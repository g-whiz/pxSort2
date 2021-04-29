#ifndef PXSORT2_EFFECT_H
#define PXSORT2_EFFECT_H

#include <pxsort/Segment.h>

namespace pxsort {

    /**
     * A PixelComparator is a callable object that imposes some ordering on
     *   Pixels.
     * The value returned by a PixelComparator is negative, zero, or positive
     *   when the first pixel is less-than, equal-to, or greater-than the second
     *   pixel, respectively.
     */
    typedef std::function<int(Pixel, Pixel)> PixelComparator;

    /**
     * Base interface for effects. An Effect is associated with a specific Segment.
     * Each time that apply() is called, an Effect will apply its effect to the
     *   given Segment, mutating the underlying Image in the process.
     *
     * Although both attach and apply take a Segment as a parameter, the same Segment
     *   that was attached to an effect must be given to apply. Not doing so
     *   may result in unspecified behaviour.
     * In practice, apply should never be called explicitly by the library user.
     * Instead, it is called by a Segment through a double-dispatch mechanism.
     */
    class Effect {
    public:
        /**
         * Associates this effect with the given Segment. This method should be
         * called once at initialization.
         *
         * The purpose of this method is to initialize any internal state in an
         *   effect using the information about the given Segment (i.e. its size).
         * @param tile
         */
        virtual void attach(Segment &tile) = 0;

        /**
         * Applies this effect to a Segment
         * @param tile
         */
        virtual void apply(Segment &tile) = 0;

        /**
         * Returns a copier of this effect.
         * @return
         */
        virtual std::unique_ptr<Effect> clone() = 0;

        Effect(const ChannelSkew &skew,
               const Segment::Traversal traversal);

    protected:
        const ChannelSkew skew;
        const Segment::Traversal traversal;
    };
}




#endif //PXSORT2_EFFECT_H
