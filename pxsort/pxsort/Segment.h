#ifndef PXSORT2_SEGMENT_H
#define PXSORT2_SEGMENT_H

#include <pxsort/common.h>
#include "Effect.h"

/**
 * Interface for accessing a segment of an image.
 *
 * The primary purpose of the Segment interface is to provide Effects with a
 *   1-dimensional array-like view of some subset of an image's pixels.
 * In this sense, a Segment maps an arbitrary subset of an Image's pixels to
 *   a virtual array.
 *
 * Implementing classes must implement the forwardGetPixel, and
 *   forwardSetPixel methods, which provide the virtual array semantics
 *   used by Effects.
 */
class ps::Segment {
public:

    /** Returns the number of pixels in this Segment. */
    virtual int size() = 0;

    /**
     * Returns the skewed pixel at the given index according to the
     *   specified Traversal.
     * @param idx Index of the pixel in this Segment.
     * @param t Method of traversing the pixels in this Segment.
     * @param skew Skew to use when retrieving the specified pixel.
     */
    Pixel getPixel(int idx, SegmentTraversal t, ChannelSkew skew);

    /**
     * Sets the skewed pixel at the given index according to the
     *    specified Traversal.
     * @param idx Index of the pixel in this Segment.
     * @param t Method of traversing the pixels in this Segment.
     * @param skew Skew to use when setting the specified pixel.
     * @param px The Pixel to store in the specified index.
     */
    void setPixel(int idx, SegmentTraversal t, ChannelSkew skew,
                  const Pixel &px);

    /**
     * Associates the given Effect with this Segment.
     *
     * @param e The effect to attach.
     */
    void attach(std::unique_ptr<Effect> e);

    /**
     * Applies all Effects attached to this Segment once.
     * In particular, for each Effect e associated with this Segment t, this
     *   function calls e.apply(t).
     *
     * Note that this function will apply effects in the order that they
     *   were attached.
     */
    void applyEffects();

protected:
    /**
     * Retrieves a Pixel from this Segment's underlying Image using a FORWARD
     *   traversal strategy.
     * @param idx Index of the Pixel.
     * @param skew Skew of the Pixel
     * @return
     */
    virtual Pixel forwardGetPixel(int idx,
                                  ChannelSkew &skew) = 0;

    /**
     * Stores a Pixel from this Segment's underlying Image using a FORWARD
     *   traversal strategy.
     * @param idx Index of the Pixel.
     * @param skew Skew of the Pixel
     * @return
     */
    virtual void forwardSetPixel(int idx,
                                 ChannelSkew &skew,
                                 const Pixel &px) = 0;

    explicit Segment(std::shared_ptr<Image> img);

    /**
     * Pointer to the underlying Image data for this segment.
     */
    std::shared_ptr<Image> img;

private:
    int getForwardIndex(int idx, SegmentTraversal t);
    int btbfToForwardIdx(int idx);

    std::vector<std::unique_ptr<Effect>> effects;
};


#endif //PXSORT2_SEGMENT_H
