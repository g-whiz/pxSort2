#ifndef PXSORT2_SEGMENTATION_H
#define PXSORT2_SEGMENTATION_H

#include "common.h"


/**
 * A segmentation of an Image.
 */
class pxsort::Segmentation {
public:
    Segmentation();

    /**
     * Returns a new Segmentation containing the given segments.
     * Note: the given segments must be disjoint (i.e. they must not overlap
     *   in pixels at all), overlapping segments may result in undefined
     *   behaviour when applying effects.
     * @param segments
     * @return
     */
    static std::shared_ptr<Segmentation>
    fromSegments(std::vector<std::shared_ptr<Segment>> segments);

    /**
     * Returns the number of Segments in this Segmentation.
     */
    size_t size();

    /**
     * Attaches the given sorter to all Segments in this Segmentation.
     * @param e
     */
    void addEffect(const std::shared_ptr<Sorter>& e);

    /**
     * Attaches the given sorter to the specified segment in this Segmentation.
     * @param e
     * @param segment
     */
    void addEffect(std::shared_ptr<Sorter> e, int segment);

    /**
     * Applies the effects attached to each Segment in this Segmentation once.
     * In particular, this method calls Segment::applyEffects() on each of this
     *   Segmentation's segments.
     */
    void applyEffects();

    /**
     * Returns the ith segment in this segmentation.
     * @param i An index, with 0 <= i < this->size()
     * @return
     */
    std::shared_ptr<Segment> getSegment(int i);

private:
    /** The Segments in this Segmentation. */
    std::vector<std::shared_ptr<Segment>> segments;
};


#endif //PXSORT2_SEGMENTATION_H
