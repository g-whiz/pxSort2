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
     * Returns the number of Segments in this Segmentation.
     */
    virtual int size() = 0;

    /**
     * Attaches the given effect to all Segments in this Segmentation.
     * @param e
     */
    void addEffect(std::shared_ptr<Effect> e);

    /**
     * Attaches the given effect to the specified segment in this Segmentation.
     * @param e
     * @param segment
     */
    void addEffect(std::shared_ptr<Effect> e, int segment);

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

protected:
    /** The Segments in this Segmentation. */
    std::vector<std::shared_ptr<Segment>> segments;
};


#endif //PXSORT2_SEGMENTATION_H
