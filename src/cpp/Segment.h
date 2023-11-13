#ifndef PXSORT2_SEGMENT_H
#define PXSORT2_SEGMENT_H

#include <optional>

#include "fwd.h"
#include "Map.h"
#include "SegmentPixels.h"
#include "Skew.h"
#include "Image.h"

#include "geometry/Point.h"
#include "geometry/Polygon.h"
#include "geometry/Ellipse.h"

/**
 * An interface for reading and writing subsets of an Image's pixels.
 * The primary purpose of the Segment interface is to provide a
 *   1-dimensional array-like view of some subset of an Image's pixels.
 * This 1-dimensional view of an Image's pixels is then transformed by a Sorter.
 *
 * Hence, a Segment is responsible for masking Image pixels, and imposing a
 * total ordering upon pixel verts.
 */
class pxsort::Segment {
public:
    using CoordinateProjection = std::function<float(int32_t , int32_t)>;
    using CoordinateProjPtr = float(*)(int32_t, int32_t);

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
    Segment(int width, int height, int x0, int y0);

    /**
     * Creates a segment consisting of an arbitrary subset of the
     * verts in an image.
     *
     * @param points
     */
    explicit
    Segment(const std::vector<Point>& points);

    /**
     * Creates a segment consisting of an arbitrary subset of the
     * verts in an image.
     *
     * @param points
     */
    explicit
    Segment(std::vector<std::pair<int, int>> points);

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
                            const std::optional<Skew> &skew = {},
                            Image::Topology imTpg = Image::SQUARE) const;

    /**
     * Writes pixel pixelData for this Segment to the given image.
     * @tparam cn The number of channels in each of the the given Image's pixels
     * @param img The Image to retrieve the pixels from
     * @param traversal The method with which to traverse this segment
     */
    void putPixels(Image &img,
                   Traversal traversal,
                   const SegmentPixels &pixels,
                   Image::Topology imTpg) const;

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
     * The ordering of the pixels in the resulting segment is undefined.
     * @return
     */
    Segment operator|(const Segment &) const;

    /**
     * Returns a new Segment with a copy of the verts in this Segment
     * sorted according to the ordering imposed by given Map.
     * @param key A map from R^2 to R used to impose a linear order on
     *                 verts.
     */
    [[nodiscard]]
    Segment sorted(const CoordinateProjection &key) const;

    /**
     * Returns a new Segment with a copy of the verts in this Segment
     * sorted according to the ordering imposed by given Map.
     * @param key A map from R^2 to R used to impose a linear order on
     *                 verts.
     */
    [[nodiscard]]
    Segment sorted(float degrees) const;

    /**
     * Returns a new Segment including the verts in this Segment
     * that evaluate to true according to the given predicate.
     * @param predicate A map from R^2 to R used as a predicate to filter
     *  verts from this Segment. Negative values are interpreted as false,
     *  nonnegative values are interpreted as true.
     */
    [[nodiscard]]
    Segment filter(const CoordinateProjection &predicate) const;

    /**
     * Returns a copy of this Segment masked by the given polygon.
     * @param polygon
     * @return
     */
    [[nodiscard]]
    Segment mask(const Polygon &polygon) const;

    /**
     * Returns a copy of this Segment masked by the given ellipse.
     * @param polygon
     * @return
     */
    [[nodiscard]]
    Segment mask(const Ellipse &ellipse) const;


    /**
     * The number of pixel verts in this segment.
     * @return
     */
    [[nodiscard]]
    int size() const;

    Point operator[](int idx) const;

    /**
     * Returns a translated copy of this Segment.
     * @param dx The horizontal displacement of the translation.
     * @param dy The vertical displacement of the translation.
     * @return
     */
    [[nodiscard]]
    Segment translate(int dx, int dy) const;

    /**
     * Overloaded method.
     * @param t
     * @return
     */
    [[nodiscard]]
    Segment translate(Point t) const;

    /**
     * Partitions this Segment's pixels into n Segments by:
     * 1) computing the projection of each point in this segment
     * 2) splitting the range of projection values into n equal buckets
     * 3) bucketing points according to their projection value
     * @param cp
     * @param n
     * @return
     */
    std::vector<Segment> partition(const CoordinateProjection &cp, int n) const;

    /**
     * Partitions this Segment's pixels into n segments along parallel lines of
     * the given degrees.
     * @param degrees
     * @param n
     * @return
     */
    std::vector<Segment> partition(float degrees, int n) const;

private:
    Segment(std::shared_ptr<Point[]> points, int nPoints, Point translation);

    [[nodiscard]]
    int getIndexForTraversal(int idx, Traversal t) const;

    [[nodiscard]]
    int getBTBFIndex(int idx) const;

    Point translation;

    int _size;
    std::shared_ptr<Point[]> points;
};

#endif //PXSORT2_SEGMENT_H
