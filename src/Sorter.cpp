#include "Sorter.h"
#include "Segment.h"
#include "PixelMixer.h"
#include "PixelProjection.h"

using namespace pxsort;

class Sorter::SorterImpl {
public:
    virtual ~SorterImpl() = 0;

    virtual SegmentPixels operator()(
            const SegmentPixels& base,
            const SegmentPixels& skewed) const = 0;
};

class BucketSort : public Sorter::SorterImpl {
    using Bucket = std::pair<float, float>;

    const Map projectPixel;
    const Map mixPixels;
    const uint32_t nBuckets;

public:
    BucketSort(const Map& pixelProjection,
               const Map& pixelMixer,
               uint32_t nBuckets)
      : projectPixel(pixelProjection),
        mixPixels(pixelMixer),
        nBuckets(nBuckets){}

    SegmentPixels operator()(
            const SegmentPixels& base,
            const SegmentPixels& skewed) const override;

private:

    uint32_t bucket(float *pixel) const;
};

uint32_t BucketSort::bucket(float *pixel) const {
    float pxProj;
    projectPixel(pixel, &pxProj);

    double step = 1.0 / static_cast<double>(nBuckets);

    auto bucket = clamp<int>(static_cast<int>(pxProj / step),
                             0,
                             static_cast<int>(nBuckets) - 1);
    return bucket;
}

SegmentPixels BucketSort::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    auto nPixels = base.size();
    auto nChannels = base.pixelDepth;

    // Count pixels in each bucket.
    std::vector<uint32_t> counts(nBuckets, 0);
    for (int i = 0; i < nPixels; i++) {
        auto bkt = bucket(skewed.at(i));
        counts[bkt]++;
    }

    // Compute index of first pixel in each bucket
    std::vector<uint32_t> indices(nBuckets, 0);
    for (int bkt = 1; bkt < nBuckets; bkt++)
        indices[bkt] = counts[bkt - 1] + indices[bkt - 1];

    auto result = base.deepCopy();
    for (int i = 0; i < nPixels; i++) {
        float in[2 * nChannels];
        std::memcpy(in, base.at(i), nChannels * sizeof(float));
        std::memcpy(&in[nChannels], skewed.at(i), nChannels * sizeof(float));

        auto out = result.at(i);
        mixPixels(in, out);
    }

    return result;
}

class Heapify : public Sorter::SorterImpl {
    const Map compare;
    const Map mix;
    const std::optional<uint32_t> nIters;

public:
    Heapify(const Map& pixelComparator,
            const Map& pixelMixer,
            std::optional<uint32_t> nIters)
    : compare(pixelComparator),
      mix(pixelMixer),
      nIters(nIters) {}

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;
};

class Bubble : public Sorter::SorterImpl {
    const Map compare;
    const Map mix;
    const uint32_t nIters;

public:
    Bubble(const Map& pixelComparator,
           const Map& pixelMixer,
           uint32_t nIters)
    : compare(pixelComparator),
      mix(pixelMixer),
      nIters(nIters) {}

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;
};
