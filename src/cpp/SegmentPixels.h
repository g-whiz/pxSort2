#ifndef PXSORT_SEGMENTPIXELS_H
#define PXSORT_SEGMENTPIXELS_H

#include <cstdint>
#include <memory>
#include "fwd.h"

/**
 * Array-like container for the pixel data corresponding to a Segment in an
 * Image. Provides an
 */
class pxsort::SegmentPixels {
public:
    struct View;

    SegmentPixels();

    SegmentPixels(const SegmentPixels &other) = default;

    /**
     * Creates a SegmentPixels with uninitialized pixel data.
     * @param nPixels The number of pixels in this SegmentPixels.
     * @param depth The number of channels in a
     */
    SegmentPixels(int32_t nPixels, int32_t depth);

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
     * Returns a (shallow) copy of this SegmentPixels that restricts indexing
     * of the backing array to a filtered subset defined by a list of indices.
     * @param indices
     * @return
     */
    [[nodiscard]]
    SegmentPixels restrictToIndices(const std::vector<int> &indices) const;


    /**
     * Returns the translation of this segment's restricted index to indices of
     * pixels in the backing array.
     * @return
     */
    [[nodiscard]]
    std::vector<int> restrictionIndices() const;


    /**
     * Returns a (shallow) copy of this SegmentPixels that has no indexing
     * restrictions on the backing array.
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
    int size() const;

    /**
     * Returns the pixel depth of this SegmentPixels.
     * @return
     */
    [[nodiscard]]
    int depth() const;

    /**
     * Returns a (borrowed) pointer to the pixel safe_ptr the given index.
     * @param viewIdx
     */
    [[nodiscard]]
    float *px(int viewIdx);

    /**
     * const overload.
     * @param viewIdx
     * @return
     */
    [[nodiscard]]
    const float *px(int viewIdx) const;

    /**
     * Returns a deep copy of this SegmentPixels.
     * @return
     */
    [[nodiscard]]
    SegmentPixels deepCopy() const;

    [[nodiscard]]
    std::shared_ptr<View> _getView() const;

    void _setView(std::shared_ptr<View> v);

private:
    SegmentPixels(int nPixels,
                  int pixelDepth,
                  std::shared_ptr<float[]> pixelData,
                  std::shared_ptr<View> view);

    std::shared_ptr<float[]> pixelData;

    // # of pixels in pixelData
    int nPixels;

    // # of channels in each pixel
    int pixelDepth;

    // params for a restricted view of pixelData
    std::shared_ptr<View> view;
};

#endif //PXSORT_SEGMENTPIXELS_H
