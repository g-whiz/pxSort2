#ifndef PXSORT2_EFFECT_H
#define PXSORT2_EFFECT_H

#include "common.h"

/**
 * Base interface for effects. An Effect is associated with a specific Segment.
 * Each time that apply() is called, an Effect will applyToSegment its effect to the
 *   given Segment, mutating the underlying Image in the process.
 *
 * Although both addEffect and applyToSegment take a Segment as a parameter, the same Segment
 *   that was attached to an effect must be given to applyToSegment. Not doing so
 *   may result in unspecified behaviour.
 * In practice, applyToSegment should never be called explicitly by the library user.
 * Instead, it is called by a Segment through a double-dispatch mechanism.
 */
class pxsort::Effect {
public:
    /**
     * Associates this effect with the given Segment. This method should be
     * called once at initialization.
     *
     * The purpose of this method is to initialize any internal state in an
     *   effect using the information about the given Segment (i.e. its size).
     * @param segment
     */
    virtual void attachToSegment(Segment &segment) = 0;

    /**
     * Applies this effect to a Segment
     * @param segment
     */
    virtual void applyToSegment(Segment &segment) = 0;

    /**
     * Returns a copier of this effect.
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<Effect> clone() const = 0;

    Effect(const ChannelSkew &skew,
           SegmentTraversal traversal);

    /**
     * Integral valued (x, y) skew for channel access for each channel.
     */
    ChannelSkew skew;

    /**
     * Traversal strategy when applying effects to a segment.
     */
    SegmentTraversal traversal;
};

#endif //PXSORT2_EFFECT_H
