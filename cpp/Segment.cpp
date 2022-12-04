#include <unordered_set>
#include <cassert>
#include "Segment.h"
#include "Image.h"

using namespace pxsort;

struct SegmentPixels::View {
    virtual ~View() = 0;

    virtual size_t operator[](size_t idx) const = 0;
    [[nodiscard]]
    virtual size_t size() const = 0;
};

struct Subarray : public SegmentPixels::View {
    const size_t startIdx;
    const size_t length;

    Subarray(size_t startIdx, size_t length)
       : startIdx(startIdx), length(length) {}

private:
    size_t operator[](size_t idx) const override;
    size_t size() const override;
};

struct Filter : public SegmentPixels::View {
    const std::vector<size_t> indices;

    Filter(std::vector<size_t> indices) : indices(std::move(indices)) {}

private:
    size_t operator[](size_t idx) const override;
    size_t size() const override;
};

SegmentPixels::SegmentPixels(uint32_t nPixels, uint32_t depth)
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
        startTest(at(start), &res);

        if (res >= 0.0) break;
    }

    auto end = nPixels - 1;
    for (; end >= start; end--) {
        float res;
        endTest(at(end), &res);

        if (res >= 0.0) break;
    }
    auto length = end - start;

    return {nPixels, pixelDepth, pixelData,
            std::make_shared<Subarray>(start, length)};
}

pxsort::SegmentPixels::SegmentPixels(size_t nPixels, size_t depth,
                                     std::shared_ptr<float[]> pixelData,
                                     std::shared_ptr<View> view)
    : nPixels(nPixels), pixelDepth(depth),
    pixelData(std::move(pixelData)), view(std::move(view)) {}

SegmentPixels pxsort::SegmentPixels::filterRestriction(const Map &filterTest) const {
    std::vector<size_t> indices;
    for(int idx = 0; idx < nPixels; idx++) {
        float res;
        filterTest(at(idx), &res);
        if (res >= 0)
            indices.push_back(idx);
    }

    return {nPixels, pixelDepth, pixelData, std::make_shared<Filter>(indices)};
}

SegmentPixels pxsort::SegmentPixels::unrestricted() const {
    return {nPixels, pixelDepth, pixelData,
            std::make_shared<Subarray>(0, nPixels)};
}

uint32_t pxsort::SegmentPixels::size() const {
    return view->size();
}

float *pxsort::SegmentPixels::at(size_t viewIdx) const {
    auto trueIdx = (*view)[viewIdx];
    auto scaledIdx = trueIdx * pixelDepth;
    return &pixelData[scaledIdx];
}

SegmentPixels pxsort::SegmentPixels::deepCopy() const {
    std::shared_ptr<float[]> dataCopy(new float[nPixels * pixelDepth]);
    std::copy_n(this->pixelData.get(), nPixels * pixelDepth, dataCopy.get());



    return {nPixels, pixelDepth, std::move(dataCopy), view};
}

size_t Subarray::operator[](size_t idx) const {
#ifdef PXSORT_DEBUG
    assert(idx >= 0);
    assert(idx < length);
#endif // PXSORT_DEBUG
    return startIdx + idx;
}

size_t Subarray::size() const {
    return length;
}

size_t Filter::operator[](size_t idx) const {
#ifdef PXSORT_DEBUG
    assert(idx >= 0);
    assert(idx < indices.size());
#endif // PXSORT_DEBUG
    return indices[idx];
}

size_t Filter::size() const {
    return indices.size();
}

int Segment::getIndexForTraversal(int idx, Traversal t) const {
    idx = PXSORT_MODULO(idx, this->size());
    switch (t) {
        case REVERSE:
            return (this->size() - 1) - idx;

        case BINARY_TREE_BREADTH_FIRST:
            return getBTBFIndex(idx);

        case FORWARD:
        default:
            return idx;  // assume FORWARD if we get an invalid Traversal
    }
}

int Segment::getBTBFIndex(int idx) const {

    int depth = PXSORT_LOG_2(idx) - 1;
    int nSubtrees = 1 << depth;
    int subtree = PXSORT_MODULO((idx + 1), nSubtrees);

    int fullHeight = PXSORT_LOG_2(this->size()) - 1;
    int subtreeHeight = fullHeight - depth;

    int subtreeSize = (1 << subtreeHeight) - 1;
    int subtreeLoIdx = subtree * subtreeSize;

    int forwardIdx = subtreeHeight == 1 ? (2 * subtree)
                                        : (subtreeLoIdx + (subtreeSize / 2));
    return forwardIdx;
}

Segment::Segment(uint32_t width, uint32_t height, uint32_t x0, uint32_t y0)
  : pxCoords() {
    auto *mutPxCoords =
            const_cast<std::vector<Coordinates> *>(&pxCoords);
    for (int dx = 0; dx < width; dx++)
        for (int dy = 0; dy < height; dy++)
            mutPxCoords->push_back({x0 + dx, y0 + dy});
}

Segment::Segment(std::vector<Coordinates> pixelCoordinates)
    : pxCoords(std::move(pixelCoordinates)) {}

Segment::Segment(std::vector<Coordinates> pixelCoordinates,
                 const std::optional<Map>& key) :
        pxCoords(std::move(pixelCoordinates)), key(key) {}

static void noSkew(float *in, int inLen, float *out, float outLen) {
    out[0] = in[0];
    out[1] = in[1];
}

SegmentPixels Segment::getPixels(const Image &img,
                                 Segment::Traversal traversal,
                                 const std::optional<Map>& channelSkew) const {
    auto skew = channelSkew.value_or(Map(noSkew, 3, 2));

    auto nPixels = pxCoords.size();
    SegmentPixels segPx(nPixels, img.depth);

    for (int i = 0; i < nPixels; i++) {
        auto idx = getIndexForTraversal(i, traversal);

        const auto &[x, y] = pxCoords[idx];
        float *pixel = segPx.at(idx);

        for (int cn = 0; cn < img.depth; cn++) {
            float skewIn[] = {static_cast<float>(x),
                              static_cast<float>(y),
                              static_cast<float>(cn)};
            float skewOut[2];
            skew(skewIn, skewOut);
            pixel[cn] = img.at(skewOut[0], skewOut[1], cn);
        }
    }

    return segPx;
}

uint32_t Segment::size() const {
    return pxCoords.size();
}

void Segment::putPixels(Image &img,
                        Segment::Traversal traversal,
                        const SegmentPixels &segPx) const {
    const SegmentPixels fullPx = segPx.unrestricted();
#ifdef PXSORT_DEBUG
    assert(segPx.pixelDepth == img.depth);
    assert(size() == fullPx.size());
#endif

    for (int i = 0; i < size(); i++) {
        auto idx = getIndexForTraversal(i, traversal);
        const auto &[x, y] = pxCoords[idx];

        float *segPixel = fullPx.at(idx);
        float *imgPixel = img.ptr(x, y);

        std::copy_n(segPixel, img.depth, imgPixel);
    }
}

template <>
struct std::hash<Segment::Coordinates> {
    std::size_t operator()(Segment::Coordinates const& coords) const noexcept {
        uint64_t x = std::get<0>(coords);
        uint64_t y = std::get<1>(coords);
        return std::hash<uint64_t>()(x | (y << 32));
    }
};

Segment Segment::operator-(const Segment &other) const {
    std::unordered_set<Coordinates> otherCoords(other.pxCoords.begin(),
                                                other.pxCoords.end());
    std::vector<Coordinates> difference;
    for (auto &coords : this->pxCoords) {
        if (!otherCoords.contains(coords)) {
            difference.push_back(coords);
        }
    }
    return {difference, key};
}

Segment Segment::operator&(const Segment &other) const {
    std::unordered_set<Coordinates> otherCoords(other.pxCoords.begin(),
                                                other.pxCoords.end());
    std::vector<Coordinates> intersection;
    for (auto &coords : this->pxCoords) {
        if (otherCoords.contains(coords)) {
            intersection.push_back(coords);
        }
    }
    return {intersection, key};
}

struct CoordinateComparator {
    using CompareFunc = std::function<bool (const Segment::Coordinates&,
                                            const Segment::Coordinates&)>;

    const CompareFunc less;

    bool operator()(
            const Segment::Coordinates& lhs,
            const Segment::Coordinates& rhs) const {
        return less(lhs, rhs);
    }

    static
    bool defaultLess(
            const Segment::Coordinates& lhs,
            const Segment::Coordinates& rhs) noexcept {
        auto &[lx, ly] = lhs;
        auto &[rx, ry] = rhs;
        return ly < ry || (ly == ry && lx < rx);
    }

    static
    CompareFunc wrapMap(const Map &map) {
        return [=](const Segment::Coordinates& lhs,
                   const Segment::Coordinates& rhs) {
            auto &[lx, ly] = lhs;
            auto &[rx, ry] = rhs;

            float lhsF[] = {static_cast<float>(lx), static_cast<float>(ly)};
            float lhsKey;
            float rhsF[] = {static_cast<float>(rx), static_cast<float>(ry)};
            float rhsKey;

            map(lhsF, &lhsKey);
            map(rhsF, &rhsKey);

            return lhsKey < rhsKey;
        };
    }

    CoordinateComparator() : less(defaultLess) {}

    explicit CoordinateComparator(std::optional<Map> keyMap)
      : less(keyMap.has_value() ? wrapMap(keyMap.value()) : defaultLess) {}
};

Segment Segment::operator|(const Segment &that) const {
    std::set<Coordinates, CoordinateComparator>
            unionSet(CoordinateComparator(this->key));
    unionSet.insert(this->pxCoords.begin(), this->pxCoords.end());
    unionSet.insert(that.pxCoords.begin(), that.pxCoords.end());

    std::vector<Coordinates> unionVec(unionSet.begin(), unionSet.end());
    return {unionVec, key};
}

Segment Segment::sorted(const Map &coordKey) const {
    // optimization to avoid re-sorting with the same key.
    if (key.has_value() && key.value() == coordKey) return *this;

    std::vector<Coordinates> sortedCoords(pxCoords.begin(), pxCoords.end());
    CoordinateComparator less(coordKey);
    std::sort(sortedCoords.begin(), sortedCoords.end(), less);

    return {sortedCoords, coordKey};
}

Segment Segment::filter(const Map &coordPred) const {
    std::vector<Coordinates> filteredCoords;
    for (auto &cInt : pxCoords) {
        auto &[x, y] = cInt;
        float cFloat[] = {static_cast<float>(x), static_cast<float>(y)};
        float cPred;

        coordPred(cFloat, &cPred);
        if (cPred >= 0)
            filteredCoords.push_back(cInt);
    }

    return {filteredCoords, key};
}

Segment::Coordinates Segment::operator[](uint32_t idx) const {
    return pxCoords[PXSORT_MODULO(idx, this->size())];
}


