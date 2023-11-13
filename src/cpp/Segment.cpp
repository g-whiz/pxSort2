#include <set>
#include <unordered_set>
#include <cassert>
#include <Eigen/Geometry>
#include <utility>

#include "Segment.h"
#include "Image.h"
#include "util.h"

using namespace pxsort;
using namespace Eigen;

int Segment::getIndexForTraversal(int idx, Traversal t) const {
    idx = modulo(idx, this->size());
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

    int const depth = log2(idx) - 1;
    int const nSubtrees = 1 << depth;
    int const subtree = modulo((idx + 1), nSubtrees);

    int const fullHeight = log2(this->size()) - 1;
    int const subtreeHeight = fullHeight - depth;

    int const subtreeSize = (1 << subtreeHeight) - 1;
    int const subtreeLoIdx = subtree * subtreeSize;

    int const forwardIdx = subtreeHeight == 1 ? (2 * subtree)
                                        : (subtreeLoIdx + (subtreeSize / 2));
    return forwardIdx;
}

Segment::Segment(int width, int height, int x0, int y0)
  : points(new Point[width * height]),
    _size(width * height), translation(x0, y0) {
    int i = 0;
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            points[i] = {x, y};
            i++;
        }
}

Segment::Segment(const std::vector<Point>& points)
    : points(new Point[points.size()]),
      _size(points.size()), translation{0, 0} {
    for (int i = 0; i < size(); i++) {
        this->points[i] = points[i];
    }
}

SegmentPixels Segment::getPixels(const Image &img,
                                 Segment::Traversal traversal,
                                 const std::optional<Skew>& _skew,
                                 Image::Topology imTpg) const {
    auto skew = _skew.value_or(Skew());

    using getPx_t = float*(*)(Image&, const Point&);
    getPx_t getPx = (imTpg == Image::SQUARE) ? safe_ptr<Image::SQUARE>
                                             : safe_ptr<Image::TORUS>;

    SegmentPixels segPx(size(), img.depth);

    #pragma omp parallel for default(none) \
            shared(img, traversal, segPx, skew, getPx)
    for (int i = 0; i < size(); i++) {
        auto idx = getIndexForTraversal(i, traversal);

        auto pt = points[idx] + translation;
        float *pixel = segPx.px(idx);

        #pragma omp simd
        for (int cn = 0; cn < img.depth; cn++) {
            auto skewedPt = skew(pt, cn);
            pixel[cn] = getPx((Image &) img, skewedPt)[cn];
        }
    }

    return segPx;
}

int Segment::size() const {
    return _size;
}

void Segment::putPixels(Image &img,
                        Segment::Traversal traversal,
                        const SegmentPixels &segPx,
                        Image::Topology imTpg) const {
    const SegmentPixels fullPx = segPx.unrestricted();

#ifdef PXSORT_DEBUG
    assert(segPx.depth() == img.depth);
    assert(size() == fullPx.size());
#endif

    using getPx_t = float*(*)(Image&, const Point&);
    getPx_t getPx = (imTpg == Image::SQUARE) ? safe_ptr<Image::SQUARE>
                                             : safe_ptr<Image::TORUS>;


#pragma omp parallel for default(none) shared(img, traversal, fullPx, getPx)
    for (int i = 0; i < size(); i++) {
        auto idx = getIndexForTraversal(i, traversal);
        const auto pt = points[idx] + translation;

        const float *segPixel = fullPx.px(idx);
        float *imgPixel = getPx(img, pt);

        std::copy_n(segPixel, img.depth, imgPixel);
    }
}

Segment Segment::operator-(const Segment &other) const {
    std::unordered_set<Point> otherPointSet;
    for (int i = 0; i < other.size(); i++)
        otherPointSet.insert(other.points[i] + other.translation);

    std::vector<Point> difference;
    for (int i = 0; i < size(); i++) {
        if (!otherPointSet.contains(points[i] + translation)) {
            difference.push_back(points[i]);
        }
    }

    return Segment(difference).translate(translation);
}

Segment Segment::operator&(const Segment &other) const {
    std::unordered_set<Point> otherPointSet;
    for (int i = 0; i < other.size(); i++)
        otherPointSet.insert(other.points[i] + other.translation);

    std::vector<Point> intersection;
    for (int i = 0; i < size(); i++) {
        if (otherPointSet.contains(points[i] + translation)) {
            intersection.push_back(points[i]);
        }
    }
    return Segment(intersection).translate(translation);
}

struct CoordinateComparator {
    using CompareFunc = std::function<bool (const Point&,
                                            const Point&)>;

    CompareFunc less;

    bool operator()(
            const Point& lhs,
            const Point& rhs) const {
        return less(lhs, rhs);
    }

    static
    bool defaultLess(
            const Point& lhs,
            const Point& rhs) noexcept {
        auto lx = lhs.x();
        auto ly = lhs.y();
        auto rx = rhs.x();
        auto ry = rhs.y();
        return ly < ry || (ly == ry && lx < rx);
    }

    static
    CompareFunc wrapCP(const Segment::CoordinateProjection &cp) {
        return [=](const Point& lhs,
                   const Point& rhs) {
            float left = cp(lhs.x(), lhs.y());
            float right = cp(rhs.x(), rhs.y());

            return left < right;
        };
    }

    CoordinateComparator() : less(defaultLess) {}

    explicit CoordinateComparator(
            std::optional<Segment::CoordinateProjection> key)
      : less(key.has_value() ? wrapCP(key.value()) : defaultLess) {}
};

Segment Segment::operator|(const Segment &other) const {
    std::unordered_set<Point> unionSet;
    for (int i = 0; i < size(); i++)
        unionSet.insert(points[i] + translation);
    for (int i = 0; i < other.size(); i++)
        unionSet.insert(other.points[i] + other.translation);

    std::vector<Point> const unionVec(unionSet.begin(), unionSet.end());
    return Segment(unionVec);
}

Segment Segment::sorted(const CoordinateProjection &key) const {
    const std::shared_ptr<Point[]> sortedPoints(new Point[size()]);
    for (int i = 0; i < size(); i++)
        sortedPoints[i] = points[i];
    CoordinateComparator const less(key);
    std::sort(sortedPoints.get(), sortedPoints.get() + size(), less);

    return {sortedPoints, size(), translation};
}

Segment Segment::filter(const CoordinateProjection &predicate) const {
    std::vector<Point> filteredPoints;
    for (int i = 0; i < size(); i++) {
        auto pt = points[i] + translation;
        if (predicate(pt.x(), pt.y()) >= 0)
            filteredPoints.push_back(points[i]);
    }

    return Segment(filteredPoints).translate(translation);
}

Point Segment::operator[](int idx) const {
    return points[modulo(idx, size())] + translation;
}

Segment Segment::translate(int dx, int dy) const {
    return translate({dx, dy});
}

Segment Segment::translate(Point t) const {
    return {points, size(), translation + t};
}

Segment::Segment(std::shared_ptr<Point[]> points,
                 int nPoints, Point translation)
    : points(std::move(points)), _size(nPoints), translation(std::move(translation)) {}

Segment Segment::mask(const Polygon &polygon) const {
    std::vector<Point> maskedPoints;
    std::vector<bool> mask(size());
    #pragma omp parallel for default(none) shared(polygon, mask)
    for (int i = 0; i < size(); i++)
        mask[i] = polygon.containsPoint(points[i] + translation);

    for (int i = 0; i < size(); i++)
        if (mask[i])
            maskedPoints.push_back(points[i]);

    return Segment(maskedPoints).translate(translation);
}

Segment Segment::mask(const Ellipse &ellipse) const {
    std::vector<Point> maskedPoints;
    for (int i = 0; i < size(); i++) {
        if (ellipse.containsPoint(points[i] + translation)) {
            maskedPoints.push_back(points[i]);
        }
    }
    return Segment(maskedPoints).translate(translation);
}

Segment::Segment(std::vector<std::pair<int, int>> points)
        : points(new Point[points.size()]),
          _size(points.size()), translation{0, 0} {
    for (int i = 0; i < size(); i++) {
        auto &[x, y] = points[i];
        this->points[i] = {x, y};
    }
}

Hyperplane<float, 2> getHyperplaneForAngle(float degrees){
    // pick a hyperplane guaranteed to lie outside of image boundaries
    float const radians = deg2rad(degrees);
    auto const rot = Rotation2Df(radians);
    float const frac = ((degrees / 360.0) + 1.0) / 360.0;

    Point2f const p = rot * Point2f(1, 0);
    Point2f delta;

    if (degrees < 0.25)
        delta = {0, 0};
    else if (degrees < 0.5)
        delta = {IMAGE_MAX_WIDTH, 0};
    else if (degrees < 0.75)
        delta = {IMAGE_MAX_WIDTH, IMAGE_MAX_HEIGHT};
    else
        delta = {0, IMAGE_MAX_HEIGHT};

    return Hyperplane<float, 2>::Through(p + delta, delta);
}

Segment Segment::sorted(float degrees) const {
    // get perpendicular hyperplane and sort according to distance from it
    auto const h = getHyperplaneForAngle(degrees + 90);
    CoordinateProjection const key = [&](int x, int y) {
        return abs(h.signedDistance(Vector2f(x, y)));
    };
    return this->sorted(key);
}

std::vector<Segment> Segment::partition(float degrees, int n) const {
    auto const h = getHyperplaneForAngle(degrees);
    CoordinateProjection const cp = [&](int x, int y){
        return h.signedDistance(Vector2f(x, y));
    };

    return partition(cp, n);
}

std::vector<Segment> Segment::partition(const Segment::CoordinateProjection &cp,
                                        int n) const
{
    n = max(1, n);

    std::vector<std::vector<Point>> partPts(n);
    float dMin = INFINITY, dMax = -INFINITY;
    float d[size()];

    #pragma omp parallel for reduction(min:dMin) reduction(max:dMax) \
            default(none) shared(d, cp)
    for (int i = 0; i < size(); i++) {
        auto &pt = points[i];
        d[i] = cp(pt.x(), pt.y());
        dMin = min(dMin, d[i]);
        dMax = max(dMax, d[i]);
    }

    float const step =
            (dMax - dMin) <= 0 ? 1 : (dMax - dMin) / static_cast<float>(n);
    for (int i = 0; i < size(); i++) {
        int const part = min(n - 1, static_cast<int>((d[i] - dMin) / step));
        partPts[part].push_back(points[i]);
    }

    std::vector<Segment> parts;
    for (int i = 0; i < n; i++) {
        if (!partPts[i].empty())
            parts.push_back(Segment(partPts[i]).translate(translation));
    }
    return parts;
}


