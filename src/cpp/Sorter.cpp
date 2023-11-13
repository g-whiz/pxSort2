#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include "Sorter.h"
#include "Segment.h"
#include "util.h"

using namespace pxsort;

class Sorter::SorterImpl {
public:
    virtual ~SorterImpl() = default;

    virtual SegmentPixels operator()(
            const SegmentPixels& base,
            const SegmentPixels& skewed) const = 0;
};

class BucketSort : public Sorter::SorterImpl {
    const Map projectPixel;
    const Map mixPixels;
    const int32_t nBuckets;

public:
    BucketSort(const Map& pixelProjection,
               const Map& pixelMixer,
               int32_t nBuckets)
      : projectPixel(pixelProjection),
        mixPixels(pixelMixer),
        nBuckets(nBuckets){}

    ~BucketSort() override = default;

    SegmentPixels operator()(
            const SegmentPixels& base,
            const SegmentPixels& skewed) const override;
};

int bucket(const float *pixel, const Map& projectPixel, int nBuckets) {
    float pxProj;
    projectPixel(pixel, &pxProj);

    double const step = 1.0 / static_cast<double>(nBuckets);

    auto n_steps = static_cast<int>(floor(pxProj / step));
    auto bucket = clamp<int>(n_steps, 0, nBuckets - 1);

    return bucket;
}

SegmentPixels bucketSort(const SegmentPixels &base,
                         const SegmentPixels &skewed,
                         const Map& projectPixel,
                         const Map& mixPixels,
                         int32_t nBuckets) {
    const int nPixels = base.size();
    const int nChannels = base.depth();

    int bkt[nPixels];
    int counts[nBuckets];
#pragma omp simd
    for (int i = 0; i < nBuckets; i++)
        counts[i] = 0;

#pragma omp parallel for default(none) \
            shared(nPixels, skewed, bkt, nBuckets, projectPixel) \
            reduction(+:counts[:nBuckets])
    for (int i = 0; i < nPixels; i++) {
        bkt[i] = bucket(skewed.px(i), projectPixel, nBuckets);
        counts[bkt[i]]++;
    }

    // Compute index of first pixel in each bucket
    int indices[nBuckets];
    for (int i = 0; i < nBuckets; i++)
        indices[i] = 0;

    for (int b = 1; b < nBuckets; b++)
        indices[b] = counts[b - 1] + indices[b - 1];

    SegmentPixels result = base.deepCopy();
    float inPx[2 * nChannels];
    float outPx[2 * nChannels];
    #pragma omp parallel for default(none) private(inPx, outPx) \
            shared(nPixels, nChannels, indices, bkt, result, skewed, mixPixels)
    for (int iBase = 0; iBase < nPixels; iBase++) {
        int bkt_iBase = bkt[iBase];
        int iSorted;

        #pragma omp atomic capture
        iSorted = indices[bkt_iBase]++;

        std::copy_n(result.px(iSorted), nChannels, inPx);
        std::copy_n(skewed.px(iBase), nChannels, &inPx[nChannels]);

        mixPixels(inPx, outPx);

        std::copy_n(outPx, nChannels, result.px(iSorted));
    }

    return result;
}

SegmentPixels BucketSort::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    return bucketSort(base, skewed, projectPixel, mixPixels, nBuckets);
}

class Heapify : public Sorter::SorterImpl {
    const Map project;
    const Map mix;

public:
    Heapify(Map  pixelProjection,
            Map  pixelMixer)
    : project(std::move(pixelProjection)),
      mix(std::move(pixelMixer)) {}

    ~Heapify() override = default;

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
    long nPixels = base.size();
    long nChannels = base.depth();

    SegmentPixels result = skewed.deepCopy();
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
            project(result.px(root), &rootProj);

            float leftProj = -INFINITY;
            if (left < nPixels)
                project(result.px(left), &leftProj);

            float rightProj = -INFINITY;
            if (right < nPixels)
                project(result.px(right), &rightProj);

            auto largest = rootProj > leftProj ? (rootProj > rightProj ? root
                                                                       : right)
                                               : (leftProj > rightProj ? left
                                                                       : right);

            // If the largest of the root and its left and right children is
            // not the root, then we need to do a swap (mixPixels in this context)
            // and continue bubbling down.
            float inPx[2 * nChannels];
            float outPx[2 * nChannels];
            if (largest != root) {
                std::copy_n(result.px(root), nChannels, inPx);
                std::copy_n(result.px(largest), nChannels, &inPx[nChannels]);

                mix(inPx, outPx);

                std::copy_n(outPx, nChannels, result.px(root));
                std::copy_n(&outPx[nChannels], nChannels, result.px(largest));

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

    ~Bubble() override = default;

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;
};

SegmentPixels Bubble::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    auto nPixels = base.size();
    auto nChannels = base.depth();
    const int maxPasses = fraction * static_cast<double>(nPixels);

    // optimization to avoid quadratic calls to potentially expensive
    // projection routines
    const std::unique_ptr<float[]> proj(new float[nPixels]);
    #pragma omp parallel for default(none) shared(nPixels, base, proj)
    for (int i = 0; i < nPixels; i++)
        project(base.px(i), &(proj[i]));

    SegmentPixels result = skewed.deepCopy();
    int32_t passes = 0;
    int32_t n = nPixels - 1;
    float inPx[2 * nChannels];
    float outPx[2 * nChannels];
    do {
        int32_t newN = 0;
        for (int i = 1; i < n; i++) {
            if (proj[i] < proj[i - 1]) {
                auto lo = proj[i];
                proj[i] = proj[i - 1];
                proj[i - 1] = lo;
                newN = i;

                std::copy_n(result.px(i - 1), 2 * nChannels, inPx);
                mix(inPx, outPx);
                std::copy_n(outPx, 2 * nChannels, result.px(i - 1));
            }
        }
        n = newN;
        passes++;
    } while (n > 1 && passes < maxPasses);

    return result;
}


struct PseudoBubble : public Sorter::SorterImpl {
    PseudoBubble(Map pixelProjection, Map pixelMixer,
                 double fraction, int maxBuckets)
            : projectPixel(std::move(pixelProjection)),
              mixPixels(std::move(pixelMixer)),
              fraction(clamp<double>(fraction, 0.0, 1.0)),
              fineStep(1.0 / static_cast<float>(maxBuckets)),
              maxBuckets(maxBuckets) {}

    ~PseudoBubble() override = default;

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;

private:
    Map projectPixel;
    Map mixPixels;
    double fraction;

    int maxBuckets;
    float fineStep;
};

// return value:
//  - == 0 when d(x, a) == d(x, b)
//  -  < 0 when d(x, a) < d(x, b)
//  -  > 0 when d(x, a) > d(x, b)
inline int cmpAbsDist(int x, int a, int b) {
    return abs(x - a) - abs(x - b);
}

SegmentPixels PseudoBubble::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    int const nPx = base.size();
    int const nCh = base.depth();

    std::unique_ptr<int[]> const initBkt(new int[nPx]);
    int initCounts[maxBuckets];
    #pragma omp simd
    for (int i = 0; i < maxBuckets; i++)
        initCounts[i] = 0;

    #pragma omp parallel for default(none) \
            reduction(+:initCounts[:maxBuckets]) \
            shared(nPx, skewed, initBkt)
    for (int i = 0; i < nPx; i++) {
        float pxProj;
        projectPixel(skewed.px(i), &pxProj);

        initBkt[i] = clamp(static_cast<int>(std::floor(pxProj / fineStep)),
                           0, maxBuckets - 1);
        initCounts[initBkt[i]]++;
    }

    int target = std::ceil(static_cast<double>(nPx) * fraction);
    int minBkt = maxBuckets;
    for (int size = 0; minBkt > 0;) {
        // if we are on target, or would exceed target next iteration, stop
        if (size >= target
            || (minBkt > 0 && (size + initCounts[minBkt - 1] > target)))
            break;
        minBkt--;
        size += initCounts[minBkt];
    }

    const std::unique_ptr<int[]> bkt(new int[nPx]);
    const int nBkts = 1 + (maxBuckets - minBkt);
    int bktCounts[nBkts];
    #pragma omp simd
    for (int i = 0; i < nBkts; i++)
        bktCounts[i] = 0;

    #pragma omp parallel for default(none) \
            reduction(+:bktCounts[:nBkts]) \
            shared(nPx, initBkt, bkt, minBkt, nBkts)
    for (int i = 0; i < nPx; i++) {
        int const bb = max(0, 1 + (initBkt[i] - minBkt));
        bkt[i] = bb;
        bktCounts[bb]++;
    }

    // Compute index of first pixel in each bucket
    int bktStarts[nBkts];
    bktStarts[0] = 0;
    for (int b = 1; b < nBkts; b++)
        bktStarts[b] = bktCounts[b - 1] + bktStarts[b - 1];


    const std::unique_ptr<int[]> sortedIdx(new int[nPx]);
    // unavoidable / non-parallelizable loop?
    // want to preserve "unsorted" pixels' original order
    for (int i = 0; i < nPx; i++) {
        auto b = bkt[i];
        sortedIdx[i] = bktStarts[b]++;
    }

    SegmentPixels result = base.deepCopy();
    float inPx[2 * nCh];
    float outPx[2 * nCh];
    #pragma omp parallel for default(none) private(inPx, outPx) \
            shared(nPx, nCh, sortedIdx, result, skewed)
    for (int iBase = 0; iBase < nPx; iBase++) {
        int iSorted = sortedIdx[iBase];

        std::copy_n(result.px(iSorted), nCh, inPx);
        std::copy_n(skewed.px(iBase), nCh, &inPx[nCh]);

        mixPixels(inPx, outPx);

        std::copy_n(outPx, nCh, result.px(iSorted));
    }

    return result;
}

struct PseudoBubble2 : public Sorter::SorterImpl {
    PseudoBubble2(Map pixelProjection, Map pixelMixer,
                  double fraction, int maxBuckets)
            : projectPixel(std::move(pixelProjection)),
              mixPixels(std::move(pixelMixer)),
              fraction(clamp<double>(fraction, 0.0, 1.0)),
              fineStep(1.0 / static_cast<float>(maxBuckets)),
              maxBuckets(maxBuckets) {}

    ~PseudoBubble2() override = default;

    SegmentPixels operator()(
            const SegmentPixels &base,
            const SegmentPixels &skewed) const override;

private:
    Map projectPixel;
    Map mixPixels;
    double fraction;

    int maxBuckets;
    float fineStep;
};

SegmentPixels PseudoBubble2::operator()(
        const SegmentPixels &base,
        const SegmentPixels &skewed) const {
    int const nPx = base.size();
    int const nCh = base.depth();

    std::unique_ptr<int[]> const initBkt(new int[nPx]);
    int initCounts[maxBuckets];
#pragma omp simd
    for (int i = 0; i < maxBuckets; i++)
        initCounts[i] = 0;

#pragma omp parallel for default(none) \
            reduction(+:initCounts[:maxBuckets]) \
            shared(nPx, skewed, initBkt)
    for (int i = 0; i < nPx; i++) {
        float pxProj;
        projectPixel(skewed.px(i), &pxProj);

        initBkt[i] = clamp(static_cast<int>(std::floor(pxProj / fineStep)),
                           0, maxBuckets - 1);
        initCounts[initBkt[i]]++;
    }

    int target = std::ceil(static_cast<double>(nPx) * fraction);
    int minBkt = maxBuckets;
    for (int size = 0; minBkt > 0;) {
        // if we are on target, or would exceed target next iteration, stop
        if (size >= target
            || (minBkt > 0 && (size + initCounts[minBkt - 1] > target)))
            break;
        minBkt--;
        size += initCounts[minBkt];
    }

    int endcap = nPx - target;
    std::vector<int> filterIdx;
    for (int i = 0; i < endcap; i++) {
        if (initBkt[i] >= minBkt)
            filterIdx.push_back(i);
    }
    for (int i = endcap; i < nPx; i++)
        filterIdx.push_back(i);

    auto rBase = base.restrictToIndices(filterIdx);
    SegmentPixels rSkew = skewed;
    rSkew._setView(rBase._getView());

    auto result = bucketSort(rBase, rSkew, projectPixel, mixPixels, maxBuckets);
    result._setView(base._getView());

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

Sorter pxsort::Sorter::bubble(const Map &pixelProjection,
                              const Map &pixelMixer,
                              double fraction) {
    assert(2 * pixelProjection.inDim == pixelMixer.inDim);
    assert(pixelProjection.outDim == 1);
    assert(pixelMixer.inDim == pixelMixer.outDim);

    auto depth = pixelProjection.inDim;
    return {
            depth,
            std::make_shared<Bubble>(pixelProjection, pixelMixer, fraction)};
}

pxsort::Sorter::Sorter(int32_t pixelDepth, std::shared_ptr<SorterImpl> pImpl)
  : pixelDepth(pixelDepth), pImpl(std::move(pImpl)) {}

SegmentPixels pxsort::Sorter::operator()(
        const SegmentPixels &basePixels,
        const SegmentPixels &skewedPixels) const {
    assert(basePixels.depth() == this->pixelDepth);
    assert(skewedPixels.depth() == this->pixelDepth);
    return (*pImpl)(basePixels, skewedPixels);
}

Sorter
pxsort::Sorter::pseudoBubble(const Map &pixelProjection, const Map &pixelMixer,
                             double fraction, int maxBuckets) {
    auto depth = pixelProjection.inDim;
    return {
            depth,
            std::make_shared<PseudoBubble>(pixelProjection, pixelMixer,
                                           fraction, maxBuckets)};
}
