#include <vector>
#include <cassert>
#include "SegmentPixels.h"
#include "Map.h"

using namespace pxsort;

struct SegmentPixels::View {
    virtual ~View() = default;

    virtual int operator[](int idx) const = 0;
    [[nodiscard]]
    virtual int size() const = 0;
};

struct Subarray : public SegmentPixels::View {
    const int startIdx;
    const int length;

    ~Subarray() override = default;

    Subarray(size_t startIdx, size_t length)
            : startIdx(startIdx), length(length) {}

private:
    int operator[](int idx) const override;
    int size() const override;
};

struct Filter : public SegmentPixels::View {
    const std::vector<int> indices;

    ~Filter() override = default;

    Filter(std::vector<int> indices) : indices(std::move(indices)) {}

private:
    int operator[](int idx) const override;
    int size() const override;
};

SegmentPixels::SegmentPixels(int32_t nPixels, int32_t depth)
        : nPixels(nPixels), pixelDepth(depth),
          pixelData(new float[depth * nPixels]),
          view(std::make_shared<Subarray>(0, nPixels)) {
    assert(depth > 0);
}

SegmentPixels SegmentPixels::asdfRestriction(const Map& startTest,
                                             const Map& endTest) const {
    auto start = 0;
    for (; start < nPixels; start++) {
        float res;
        startTest(px(start), &res);

        if (res >= 0.0) break;
    }

    auto end = nPixels - 1;
    for (; end >= start; end--) {
        float res;
        endTest(px(end), &res);

        if (res >= 0.0) break;
    }
    auto length = end - start;

    return {nPixels, pixelDepth, pixelData,
            std::make_shared<Subarray>(start, length)};
}

pxsort::SegmentPixels::SegmentPixels(int nPixels, int pixelDepth,
                                     std::shared_ptr<float[]> pixelData,
                                     std::shared_ptr<View> view)
        : nPixels(nPixels), pixelDepth(pixelDepth),
          pixelData(std::move(pixelData)), view(std::move(view)) {}

SegmentPixels pxsort::SegmentPixels::filterRestriction(const Map &filterTest) const {
    std::vector<int> indices;
    for(int idx = 0; idx < nPixels; idx++) {
        float res;
        filterTest(px(idx), &res);
        if (res >= 0)
            indices.push_back(idx);
    }

    return {nPixels, pixelDepth, pixelData, std::make_shared<Filter>(indices)};
}

SegmentPixels pxsort::SegmentPixels::unrestricted() const {
    return {nPixels, pixelDepth, pixelData,
            std::make_shared<Subarray>(0, nPixels)};
}

int pxsort::SegmentPixels::size() const {
    return view->size();
}

float *pxsort::SegmentPixels::px(int viewIdx) {
    auto trueIdx = (*view)[viewIdx];
    auto scaledIdx = trueIdx * pixelDepth;
    return &pixelData[scaledIdx];
}

const float *pxsort::SegmentPixels::px(int viewIdx) const {
    return ((SegmentPixels *) this)->px(viewIdx);
}


SegmentPixels pxsort::SegmentPixels::deepCopy() const {
    std::shared_ptr<float[]> dataCopy(new float[nPixels * pixelDepth]);
    std::copy_n(this->pixelData.get(), nPixels * pixelDepth, dataCopy.get());

    return {nPixels, pixelDepth, std::move(dataCopy), view};
}

SegmentPixels pxsort::SegmentPixels::restrictToIndices(
        const std::vector<int> &indices) const {
    std::vector<int> validIndices;
    for (auto &idx : indices)
        if (0 <= idx && idx < size())
            validIndices.push_back((*view)[idx]);
    return {nPixels, pixelDepth,
            pixelData, std::make_shared<Filter>(validIndices)};
}

std::vector<int> pxsort::SegmentPixels::restrictionIndices() const {
    std::vector<int> indices(view->size());
    for (int i = 0; i < view->size(); i++)
        indices[i] = (*view)[i];
    return indices;
}

pxsort::SegmentPixels::SegmentPixels()
        : SegmentPixels(1, 1) {}

int Subarray::operator[](int idx) const {
#ifdef PXSORT_DEBUG
    assert(idx >= 0);
    assert(idx < size());
#endif // PXSORT_DEBUG
    return startIdx + idx;
}

int Subarray::size() const {
    return length;
}

int Filter::operator[](int idx) const {
#ifdef PXSORT_DEBUG
    assert(idx >= 0);
    assert(idx < indices.size());
#endif // PXSORT_DEBUG
    return indices[idx];
}

int Filter::size() const {
    return indices.size();
}

int SegmentPixels::depth() const {
    return pixelDepth;
}

std::shared_ptr<Filter::View> pxsort::SegmentPixels::_getView() const {
    return view;
}

void pxsort::SegmentPixels::_setView(std::shared_ptr<View> v) {
    this->view = std::move(v);
}
