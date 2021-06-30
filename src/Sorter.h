#ifndef PXSORT2_SORTER_H
#define PXSORT2_SORTER_H

#include "common.h"

/**
 * Base interface for effects. An Sorter is associated with a specific Segment.
 * Each time that apply() is called, an Sorter will applyToSegment its sorter to the
 *   given Segment, mutating the underlying Image in the process.
 *
 * Although both addEffect and applyToSegment take a Segment as a parameter, the same Segment
 *   that was attached to an sorter must be given to applyToSegment. Not doing so
 *   may result in unspecified behaviour.
 * In practice, applyToSegment should never be called explicitly by the library user.
 * Instead, it is called by a Segment through a double-dispatch mechanism.
 */
class pxsort::Sorter {
public:
    /**
     * Associates this sorter with the given Segment. This method should be
     * called once at initialization.
     *
     * The purpose of this method is to initialize any internal state in an
     *   sorter using the information about the given Segment (i.e. its size).
     * @param segment
     */
    virtual void attachToSegment(Segment &segment) = 0;

    /**
     * Applies this sorter to a Segment
     * @param segment
     */
    virtual void applyToSegment(Segment &segment) = 0;

    /**
     * Returns a copier of this sorter.
     * @return
     */
    [[nodiscard]] virtual std::unique_ptr<Sorter> clone() const = 0;

    Sorter(const ChannelSkew &skew,
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

#endif //PXSORT2_SORTER_H
