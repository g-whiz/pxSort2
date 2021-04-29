#ifndef PXSORT2_SEGMENTATION_H
#define PXSORT2_SEGMENTATION_H

#include <pxsort/Segment.h>

namespace pxsort {

    /**
     * A segmentation of an Image.
     */
    class Segmentation {
    public:
        /**
         * Returns the number of Segments in this Segmentation.
         */
        virtual int size() = 0;

        /**
         * Attaches the given effect to all Segments in this Segmentation.
         * @param e
         */
        void addEffect(std::unique_ptr<Effect> e);

        /**
         * Attaches the given effect to the specified segment in this Segmentation.
         * @param e
         * @param segment
         */
        void addEffect(std::unique_ptr<Effect> e, int segment);

        /**
         * Applies the effects attached to each Segment in this Segmentation once.
         * In particular, this method calls Segment::applyEffects() on each of this
         *   Segmentation's segments.
         */
        void applyEffects();

    protected:
        /** The Segments in this Segmentation. */
        std::vector<std::unique_ptr<Segment>> segments;

    };

}


#endif //PXSORT2_SEGMENTATION_H
