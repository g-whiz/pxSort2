#ifndef PXSORT2_SEGMENT_H
#define PXSORT2_SEGMENT_H

#include <optional>
#include <set>
#include "common.h"
#include "Map.h"

/**
 * An interface for reading and writing subsets of an Image's pixels.
 * The primary purpose of the Segment interface is to provide a
 *   1-dimensional array-like view of some subset of an Image's pixels.
 * This 1-dimensional view of an Image's pixels is then transformed by a Sorter.
 *
 * Hence, a Segment is responsible for masking Image pixels, and imposing a
 * total ordering upon pixel coordinates.
 */
class pxsort::Segment {
public:
    using Coordinates = std::pair<uint32_t, uint32_t>;

    /** Traversal options for a Segment's pixels. */
    enum Traversal {
        FORWARD,
        REVERSE,
        BINARY_TREE_BREADTH_FIRST
    };

    Segment() = delete;

    /**
     * Copy constructor.
     * @param other
     */
    Segment(const Segment &other) = default;

    /**
     * Creates a segment that provides access to some rectangular subset of the
     * pixels in an Image.
     * @param width
     * @param height
     * @param x0
     * @param y0
     */
    Segment(uint32_t width, uint32_t height, uint32_t x0, uint32_t y0);

    /**
     * Creates a segment consisting of an arbitrary subset of the
     * pxCoords in an image.
     *
     * @param pixelCoordinates
     */
    explicit
    Segment(std::vector<Coordinates> pixelCoordinates);

    /**
     * Reads pixel pixelData for this Segment from the given image.
     * @param img The Image to retrieve the pixels from
     * @param traversal The method with which to traverse this segment
     * @param skew An optional coordinate transform to use when retrieving each
     *   channel of each pixel from the given Image. Must have the form:
     *   (x, y, cn) -> (x', y'), where cn is the channel number for retrieval.
     * @return The pixels retrieved from the given image.
     */
    [[nodiscard]]
    SegmentPixels getPixels(const Image &img,
                            Traversal traversal,
                            const std::optional<Map> &channelSkew = {}) const;

    /**
     * Writes pixel pixelData for this Segment to the given image.
     * @tparam cn The number of channels in each of the the given Image's pixels
     * @param img The Image to retrieve the pixels from
     * @param traversal The method with which to traverse this segment
     */
    void putPixels(Image &img,
                   Traversal traversal,
                   const SegmentPixels &pixels) const;

    /**
     * Returns the set-difference of the pixels in this segment and the
     * given segment.
     * The ordering of the pixels in the resulting segment is done the same as
     * is done in the left hand side Segment.
     * @return
     */
    Segment operator-(const Segment &) const;

    /**
     * Returns the intersection of the pixels in this segment and the
     * given segment.
     * The ordering of the pixels in the resulting segment is done the same as
     * is done in the left hand side Segment.
     * @return
     */
    Segment operator&(const Segment &) const;

    /**
     * Returns the union of the pixels in this segment and the given segment.
     * The ordering of the pixels in the resulting segment is done the same as
     * is done in the left hand side Segment.
     * @return
     */
    Segment operator|(const Segment &) const;

    /**
     * Returns a new Segment with a copy of the coordinates in this Segment
     * sorted according to the ordering imposed by given Map.
     * @param coordKey A map from R^2 to R used to impose a linear order on
     *                 coordinates.
     */
    [[nodiscard]]
    Segment sorted(const Map &coordKey) const;

    /**
     * Returns a new Segment including the coordinates in this Segment
     * that evaluate to true according to the given predicate.
     * @param coordPred A map from R^2 to R used as a predicate to filter
     *  coordinates from this Segment. Negative values are interpreted as false,
     *  nonnegative values are interpreted as true.
     */
    [[nodiscard]]
    Segment filter(const Map &coordPred) const;

    /**
     * The number of pixel coordinates in this segment.
     * @return
     */
    [[nodiscard]]
    uint32_t size() const;

    Coordinates operator[](int idx) const;

private:
    Segment(std::vector<Coordinates>, const std::optional<Map> &);


    [[nodiscard]]
    int getIndexForTraversal(int idx, Traversal t) const;

    [[nodiscard]]
    int getBTBFIndex(int idx) const;


    const std::vector<Coordinates> pxCoords;

    // optional key function for ordering pixel coordinates
    const std::optional<Map> key;
};


/**
 * Array-like container for the pixel data corresponding to a Segment in an
 * Image. Provides an
 */
class pxsort::SegmentPixels {
public:
    struct View;

    // # of channels in each pixel
    const uint32_t pixelDepth;

    SegmentPixels() = delete;

    SegmentPixels(const SegmentPixels &other) = default;

    SegmentPixels(SegmentPixels &&other) = default;

    /**
     * Creates a SegmentPixels with uninitialized pixel data.
     * @param nPixels The number of pixels in this SegmentPixels.
     * @param depth The number of channels in a
     */
    SegmentPixels(uint32_t nPixels, uint32_t depth);

    /**
     * Returns a (shallow) copy of this SegmentPixels that restricts indexing
     * of the backing array to a subarray defined by a pair of predicates.
     *
     * Each of the given predicates must be a Map of the form:
     *   P: [0, 1]^D -> (-∞, ∞); where D := this->pixelDepth.
     * Negative outputs are interpreted as the predicate evaluating to false
     * and nonnegative outputs are interpreted as evaluating to true.
     *
     * @param startTest A predicate to determine the start of the
     * restricted subarray. The beginning is chosen as the first pixel that
     * evaluates to true with the given predicate.
     * @param endTest A predicate to determine the end of the
     * restricted subarray. The end is chosen as the last pixel that
     * evaluates to true with the given predicate.
     * @return
     */
    [[nodiscard]]
    SegmentPixels asdfRestriction(const Map &startTest, const Map &endTest) const;

    /**
     * Returns a (shallow) copy of this SegmentPixels that restricts indexing
     * of the backing array to a filtered subset defined by a predicate.
     *
     * The given predicate must be a Map of the form:
     *   P: [0, 1]^D -> (-∞, ∞); where D := this->pixelDepth.
     * Negative outputs are interpreted as the predicate evaluating to false
     * and nonnegative outputs are interpreted as evaluating to true.
     *
     * @param filterTest A predicate to determine the pixels to include in
     * the restricted subset. Pixels are included in the subset when the given
     * predicate evaluates to true.
     * @return
     */
    [[nodiscard]]
    SegmentPixels filterRestriction(const Map &filterTest) const;


    /**
     * Returns a (shallow) copy of this SegmentPixels that unrestricts indexing
     * of the backing array.
     * @return
     */
    [[nodiscard]]
    SegmentPixels unrestricted() const;

    /**
     * Returns the number of indexable pixels in this SegmentPixels.
     * The value returned by this function depends both on the size of this
     * SegmentPixels' backing array and on
     * @return
     */
    [[nodiscard]]
    uint32_t size() const;

    /**
     * Returns a (borrowed) pointer to the pixel at the given index.
     * @param viewIdx
     */
    [[nodiscard]]
    float *at(size_t viewIdx) const;

    /**
     * Returns a deep copy of this SegmentPixels.
     * @return
     */
    [[nodiscard]]
    SegmentPixels deepCopy() const;

private:
    SegmentPixels(size_t nPixels,
                  size_t pixelDepth,
                  std::shared_ptr<float[]> pixelData,
                  std::shared_ptr<View> view);

    const std::shared_ptr<float[]> pixelData;

    // # of pixels in pixelData
    const uint32_t nPixels;

    // params for a restricted view of pixelData
    const std::shared_ptr<View> view;
};



#endif //PXSORT2_SEGMENT_H
