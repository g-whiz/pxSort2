#ifndef PXSORT_POLYGON_H
#define PXSORT_POLYGON_H

#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "fwd.h"
#include "Point.h"
#include "Modulation.h"


struct pxsort::Polygon {
    Polygon() = delete;

    /**
     * A function that modulates a polygon's boundary.
     * This function is treated as a mapping of type [0, 2π) -> [0, ∞),
     *   where the input corresponds to a an angle and the output corresponds
     *   to a scaling factor for points at that angle relative to the polygon's
     *   centroid.
     */
    using Modulator = modulator::Modulator;

    /**
     * Create a polygon with the given sequence of vertices.
     * @param vertices A sequence of at least 3 vertices defining the edges of
     *   the Polygon. All polygons are supported, including non-convex
     *   and complex polygons.
     */
    Polygon(std::vector<Point2f> vertices);

    /**
     * Create a polygon with the given sequence of vertices.
     * @param vertices A sequence of at least 3 vertices defining the edges of
     *   the Polygon. All polygons are supported, including non-convex
     *   and complex polygons.
     */
    Polygon(const std::vector<std::pair<float, float>> &vertices);

    /**
     * Create a polygon with the given sequence of vertices.
     * @param vertices A sequence of at least 3 vertices defining the edges of
     *   the Polygon. All polygons are supported, including non-convex
     *   and complex polygons.
     */
    template<typename T> requires Arithmetic<T>
    Polygon(const std::vector<Point_<T>> &vertices)
        : verts(vertices.size())
    {
        for (int i = 0; i < vertices.size(); i++) {
            verts[i] = static_cast<Point2f>(vertices[i]);
        }
    }

    /**
     * Create a regular polygon with the given radius and number of sides,
     * centered at the given point.
     * @param sides Number of sides. Must have sides >= 3.
     * @param radius Distance from the center to each vertex of the polygon.
     * @param cX x coordinate to center the polygon at.
     * @param cY x coordinate to center the polygon at.
     */
    Polygon(int sides, float radius = 1, float cX = 0, float cY = 0);

    /**
     * Returns a new Polygon that applies the given modulator to this
     * Polygon's edges.
     * @param m
     * @param vertices
     * @return
     */
    Polygon modulate(Modulator m, int vertices) const;

    /**
     * Returns a sheared copy of this polygon.
     * @param sx Horizontal shear factor.
     * @param sy Vertical sear factor.
     * @return
     */
    Polygon shear(float sx, float sy);

    /**
     * Returns a translated copy of this polygon.
     * @param dx Horizontal displacement.
     * @param dy Vertical displacement.
     * @return
     */
    [[nodiscard]]
    Polygon translate(float dx, float dy) const;

    /**
     * Returns a copy of this Polygon rotated about the origin (0 ,0).
     * @param degrees The angle to rotate this polygon by.
     * Positive: counterclockwise, negative: clockwise.
     * @return
     */
    Polygon rotate(float degrees) const;

    /**
     * Returns a scaled copy of this polygon. A negative scale factor will
     *   reflect about the corresponding axis.
     * @param sx Horizontal scale factor
     * @param sy Vertical scale factor
     * @return
     */
    Polygon scale(float sx, float sy) const;

    template<typename T> requires Arithmetic<T>
    inline bool containsPoint(const Point_<T>&pt) const {
        return _containsPoint(pt.template cast<float>());
    }

private:
    static constexpr double pi = std::numbers::pi;

    bool _containsPoint(const Point2f &p) const;
    Polygon transformed(const Eigen::AffineCompact2f &t) const;

    std::vector<Point2f> verts;
};

namespace pxsort {
    template<>
    inline bool Polygon::containsPoint(const Point2f &pt) const {
        return _containsPoint(pt);
    }
}

#endif //PXSORT_POLYGON_H
