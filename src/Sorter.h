#ifndef PXSORT2_SORTER_H
#define PXSORT2_SORTER_H

#include "common.h"
#include "Segment.h"

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
    class SorterImpl;

    Sorter(const Sorter&) = default;

    /**
     * Returns a sorted (deep) copy of the given SegmentPixels.
     * @param pixels The SegmentPixels to sort.
     * @return
     */
    [[nodiscard]]
    SegmentPixels operator()(const SegmentPixels& pixels) const;

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
            const SegmentPixels& basePixels,
            const SegmentPixels& skewedPixels) const;

    [[nodiscard]]
    static Sorter bucketSort(const Map& pixelProjection,
                             const Map& pixelMixer,
                             uint32_t nBuckets);

    [[nodiscard]]
    static Sorter heapify(const Map& pixelComparator,
                          const Map& pixelMixer,
                          std::optional<uint32_t> nIters = {});

    [[nodiscard]]
    static Sorter bubble(const Map& pixelComparator,
                         const Map& pixelMixer,
                         uint32_t nIters);

private:
    Sorter(uint32_t pixelDepth, std::shared_ptr<SorterImpl>);

    const uint32_t pixelDepth;
    const std::shared_ptr<SorterImpl> pImpl;
};

#endif //PXSORT2_SORTER_H
