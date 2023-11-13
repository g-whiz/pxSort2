#ifndef PXSORT2_SORTER_H
#define PXSORT2_SORTER_H

#include "fwd.h"
#include "Segment.h"

/**
 * A callable object that sorts arrays of pixels.
 */
class pxsort::Sorter {
public:
    class SorterImpl;

    Sorter(const Sorter &) = default;

    /**
     * Returns a (deep) copy of basePixels that has had the pixels from
     * skewedPixels sorted into it.
     * Note: base and skewed must have the same size().
     * @param basePixels The SegmentPixels to sort.
     * @param skewedPixels The skewed SegmentPixels to sort into base.
     * @return
     */
    [[nodiscard]]
    SegmentPixels operator()(
            const SegmentPixels &basePixels,
            const SegmentPixels &skewedPixels) const;

    /**
     * Returns a Sorter that efficiently sorts all pixels in a SegmentPixels.
     *
     * This Sorter uses a bucket-sort implementation, which has a runtime of
     * O(n).
     * @param pixelProjection A Map from [0, 1]^d to [0, 1] (where d is pixel
     *   depth). This Map is used to determine the order of pixels.
     * @param pixelMixer A Map from [0, 1]^2d to [0, 1]^2d (where d is pixel
     *   depth). This Map is used to combine or "swap" a pair of pixels that
     *   are being compared. Note that this version of the Sorter ignores the
     *   last d elements of a pixelMixer's output.
     * @param nBuckets The number of bins to use when binning pixels'
     * projections. Bins are equal-length sub-intervals produced by partitioning
     * [0, 1] into nBuckets segments.
     * @return
     */
    [[nodiscard]]
    static Sorter bucketSort(const Map &pixelProjection,
                             const Map &pixelMixer,
                             uint32_t nBuckets);

    /**
     * Fast approximation of a partial bubble-sort effect.
     * @param pixelProjection
     * @param pixelMixer
     * @param maxBuckets
     * @param fraction
     * @return
     */
    [[nodiscard]]
    static Sorter pseudoBubble(const Map &pixelProjection,
                               const Map &pixelMixer,
                               double fraction,
                               int maxBuckets);

    /**
     * Returns a sorter that builds a max-heap from the pixels in a
     * SegmentPixels.
     * @param pixelProjection A Map from [0, 1]^d to [0, 1] (where d is pixel
     *   depth). This Map is used to determine the order of pixels.
     * @param pixelMixer A Map from [0, 1]^2d to [0, 1]^2d (where d is pixel
     *   depth). This Map is used to combine or "swap" a pair of pixels that
     *   are being compared.
     * @return
     */
    [[nodiscard]]
    static Sorter heapify(const Map &pixelProjection,
                          const Map &pixelMixer);

    /**
     * Returns a Sorter that performs a partial bubble-sort on the given
     * SegmentPixels.
     * @param pixelProjection A Map from [0, 1]^d to [0, 1] (where d is pixel
     *   depth). This Map is used to determine the order of pixels.
     * @param pixelMixer A Map from [0, 1]^2d to [0, 1]^2d (where d is pixel
     *   depth). This Map is used to combine or "swap" a pair of pixels that
     *   are being compared. Note that this version of the Sorter ignores the
     *   last d elements of a pixelMixer's output.
     * @param fraction A number in the interval (0, 1]. Used to determine where
     * to stop in the bubble-sort process.
     * @return
     */
    [[nodiscard]]
    static Sorter bubble(const Map &pixelProjection,
                         const Map &pixelMixer,
                         double fraction);

private:
    Sorter(int32_t pixelDepth, std::shared_ptr<SorterImpl> pImpl);

    int32_t pixelDepth;
    std::shared_ptr<SorterImpl> pImpl;
};

#endif //PXSORT2_SORTER_H
