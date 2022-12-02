#include <cassert>
#include <cmath>
#include <vector>
#include "Sorter.h"
#include "Segment.h"

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
    float inPx[2 * nChannels];
    float outPx[2 * nChannels];
    for (int iRes = 0; iRes < nPixels; iRes++) {
        auto bkt = bucket(skewed.at(iRes));
        auto iSkew = indices[bkt];
        indices[bkt]++;

        std::copy_n(base.at(iRes), nChannels, inPx);
        std::copy_n(skewed.at(iSkew), nChannels, &inPx[nChannels]);

        mixPixels(inPx, outPx);

        std::copy_n(outPx, nChannels, result.at(iRes));
    }

    return result;
}

class Heapify : public Sorter::SorterImpl {
    const Map project;
    const Map mix;

public:
    Heapify(Map  pixelProjection,
            Map  pixelMixer)
    : project(std::move(pixelProjection)),
      mix(std::move(pixelMixer)) {}

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;

private:
    static inline long left_child(long idx) { return (2 * idx) + 1; };
    static inline long right_child(long idx) { return (2 * idx) + 2; };
};

SegmentPixels Heapify::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    auto nPixels = base.size();
    auto nChannels = base.pixelDepth;

    SegmentPixels result = base.deepCopy();
    for (long i = (nPixels / 2) - 1; i >= 0; i--) {
        // Perform bubble-down pass for a single element of the heap.
        // Note: the outer if statements in the loop are just bounds checks
        //       (i.e. "Is there a left/right child?").
        auto root = i;
        do {
            auto left = left_child(root);
            auto right = right_child(root);

            // Use our pixel projection to determine the "largest" pixel out of
            // the root and its left and right children (if they exist).
            float rootProj;
            project(result.at(root), &rootProj);

            float leftProj = -INFINITY;
            if (left < nPixels)
                project(skewed.at(left), &leftProj);

            float rightProj = -INFINITY;
            if (right < nPixels)
                project(skewed.at(right), &rightProj);

            auto largest = rootProj > leftProj ? (rootProj > rightProj ? root
                                                                       : right)
                                               : (leftProj > rightProj ? left
                                                                       : right);

            // If the largest of the root and its left and right children is
            // not the root, then we need to do a swap (mix in this context)
            // and continue bubbling down.
            float inPx[2 * nChannels];
            float outPx[2 * nChannels];
            if (largest != root) {
                std::copy_n(result.at(root), nChannels, inPx);
                std::copy_n(skewed.at(largest), nChannels, &inPx[nChannels]);

                mix(inPx, outPx);

                std::copy_n(outPx, nChannels, result.at(root));
                std::copy_n(&outPx[nChannels], nChannels, result.at(largest));

                root = largest;
            }
            // If the largest of the root and its left and right children is
            // the root, then we are done bubbling down this element.
            else break;

        } while (root < nPixels);
    }

    return result;
}

class Bubble : public Sorter::SorterImpl {
    const Map project;
    const Map mix;
    const double fraction;

public:
    Bubble(Map  pixelProjection,
           Map  pixelMixer,
           float fraction)
    : project(std::move(pixelProjection)),
      mix(std::move(pixelMixer)),
      fraction(clamp<float>(fraction, 0.0, 1.0)) {}

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;
};

SegmentPixels Bubble::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    auto nPixels = base.size();
    auto nChannels = base.pixelDepth;
    uint32_t maxPasses = fraction * static_cast<double>(nPixels); // NOLINT(cppcoreguidelines-narrowing-conversions)

    // optimization to avoid quadratic calls to potentially expensive project
    // functions
    std::unique_ptr<float[]> skewProj(new float[nPixels]);
    for (int i = 0; i < nPixels; i++)
        project(skewed.at(i), &skewProj[i]);

    SegmentPixels result = base.deepCopy();
    uint32_t passes = 0;
    uint32_t n = nPixels - 1;
    float baseProj;
    float inPx[2 * nChannels];
    float outPx[2 * nChannels];
    do {
        uint32_t newN = 0;
        project(base.at(n), &baseProj);
        for (int i = 1; i < n; i++) {
            if (skewProj[i] < baseProj) {
                newN = i;

                std::copy_n(skewed.at(i - 1), nChannels, inPx);
                std::copy_n(result.at(i), nChannels, inPx);
                mix(inPx, outPx);
                std::copy_n(outPx, nChannels, result.at(i));
            }
        }
        n = newN;
        passes++;
    } while (n > 1 || passes >= maxPasses);

    return result;
}

Sorter pxsort::Sorter::bucketSort(
        const Map &pixelProjection,
        const Map &pixelMixer,
        uint32_t nBuckets) {
    assert(2 * pixelProjection.inDim == pixelMixer.inDim);
    assert(pixelProjection.outDim == 1);
    assert(pixelMixer.inDim == pixelMixer.outDim);

    auto depth = pixelProjection.inDim;
    return {
        depth,
        std::make_shared<BucketSort>(pixelProjection, pixelMixer, nBuckets)};
}

Sorter pxsort::Sorter::heapify(const Map &pixelProjection,
                               const Map &pixelMixer) {
    assert(2 * pixelProjection.inDim == pixelMixer.inDim);
    assert(pixelProjection.outDim == 1);
    assert(pixelMixer.inDim == pixelMixer.outDim);

    auto depth = pixelProjection.inDim;
    return {
            depth,
            std::make_shared<Heapify>(pixelProjection, pixelMixer)};
}

Sorter pxsort::Sorter::bubble(const Map &pixelProjection, const Map &pixelMixer, double fraction) {
    assert(2 * pixelProjection.inDim == pixelMixer.inDim);
    assert(pixelProjection.outDim == 1);
    assert(pixelMixer.inDim == pixelMixer.outDim);

    auto depth = pixelProjection.inDim;
    return {
            depth,
            std::make_shared<Bubble>(pixelProjection, pixelMixer, fraction)};
}

pxsort::Sorter::Sorter(uint32_t pixelDepth, std::shared_ptr<SorterImpl> pImpl)
  : pixelDepth(pixelDepth), pImpl(std::move(pImpl)) {}

SegmentPixels pxsort::Sorter::operator()(const SegmentPixels &pixels) const {
    assert(pixels.pixelDepth == this->pixelDepth);
    return (*pImpl)(pixels, pixels);
}

SegmentPixels pxsort::Sorter::operator()(
        const SegmentPixels &basePixels,
        const SegmentPixels &skewedPixels) const {
    assert(basePixels.pixelDepth == this->pixelDepth);
    assert(skewedPixels.pixelDepth == this->pixelDepth);
    return (*pImpl)(basePixels, skewedPixels);
}
