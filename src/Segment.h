#ifndef PXSORT2_SEGMENT_H
#define PXSORT2_SEGMENT_H

#include <concepts>
#include <opencv2/core/types.hpp>
#include "common.h"
#include "Sorter.h"
#include "Image.h"

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
class pxsort::Segment {
public:
    class Pixels;

    /** Traversal options for a Segment's pixels. */
    enum Traversal {
        FORWARD,
        REVERSE,
        BINARY_TREE_BREADTH_FIRST
    };

    /** Returns the number of pixels in this Segment. */
    virtual int size() = 0;

    std::vector<Pixel> getPixels(const Image &img,
                                 Traversal traversal,
                                 const ChannelSkew &skew) const;

    void putPixels(Image &img,
                   Traversal traversal,
                   const ChannelSkew &skew,
                   const std::vector<Pixel> &pixels) const;

    /**
     * Returns the skewed pixel at the given index according to the
     *   specified Traversal.
     * @param idx Index of the pixel in this Segment.
     * @param t Method of traversing the pixels in this Segment.
     * @param skew Skew to use when retrieving the specified pixel.
     */
    Pixel getPixel(int idx, Traversal t, ChannelSkew skew);

    /**
     * Sets the skewed pixel at the given index according to the
     *    specified Traversal.
     * @param idx Index of the pixel in this Segment.
     * @param t Method of traversing the pixels in this Segment.
     * @param skew Skew to use when setting the specified pixel.
     * @param px The Pixel to store in the specified index.
     */
    void setPixel(int idx, Traversal t, ChannelSkew skew,
                  const Pixel &px);

    /**
     * Associates the given Sorter with this Segment.
     *
     * @param e The sorter to addEffect.
     */
    void addEffect(std::shared_ptr<Sorter> e);

    /**
     * Returns the total number of effects attached to this segment.
     * @return
     */
    inline size_t numEffects() { return effects.size(); }

    /**
     * Returns the ith sorter attached to this Segment.
     * @param i An index, with 0 <= i < this->numEffects()
     * @return
     */
    std::shared_ptr<Sorter> getEffect(int i);

    /**
     * Applies all Effects attached to this Segment once.
     * In particular, for each Sorter e associated with this Segment t, this
     *   function calls e.applyToSegment(t).
     *
     * Note that this function will applyToSegment effects in the order that they
     *   were attached.
     */
    void applyEffects();

    virtual ~Segment() = default;

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


private:
    int getForwardIndex(int idx, Traversal t);
    int btbfToForwardIdx(int idx);

    std::vector<std::shared_ptr<Sorter>> effects;


    class Impl;
    class Rectangle;
    class Asendorf;
    class Mask;
    class Polygon;
    class Circle;
    std::shared_ptr<Impl> pImpl;

    /*
     * todo:
     *  - change of basis matrix used to impose order on mask pixels
     *    (i.e. to determine the "forward" arrangement)
     *  - operator-(const Segment &that):
     *    returns mask that is the set diff of this segment's pixels and that's
     *  - operator+(const Segment &that):
     *    returns mask that is the union of this segment's pixels and that's
     *  - parameterizations of segments that shift them around / transform them
     *    over time
     */
};


class pxsort::Segment::Pixels{
public:
    Pixels(const Segment &seg, std::vector<Pixel> pixels);
    // todo? or maybe don't do it this way...
};


#endif //PXSORT2_SEGMENT_H
