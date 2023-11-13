#ifndef PXSORT_ELLIPSE_H
#define PXSORT_ELLIPSE_H

#include <Eigen/Geometry>
#include <optional>

#include "fwd.h"
#include "Point.h"
#include "Modulation.h"

struct pxsort::Ellipse {
    /**
     * A function that modulates an ellipse's boundary.
     * This function is treated as a mapping of type [0, 2π) -> [0, ∞),
     *   where the input corresponds to a point on the ellipse's boundary,
     *   and the output corresponds to a scaling factor for that point's radius.
     */
    using Modulator = modulator::Modulator;

    /**
     * Create a circle with the given radius, located at the given point.
     * @param radius
     * @param cx
     * @param cy
     */
    Ellipse(float radius, float cx, float cy);

    /**
     *
     * @param width
     * @param height
     * @param angle
     * @param center
     */
    Ellipse(float width, float height, float angle, float cx, float cy);

    /**
     * Returns a copy of this ellipse with the given Modulator applied to it.
     * @param m
     * @return
     */
    Ellipse modulate(Modulator m) const;

    /**
     * Returns a translated copy of this polygon.
     * @param dx Horizontal displacement.
     * @param dy Vertical displacement.
     * @return
     */
    [[nodiscard]]
    Ellipse translate(float dx, float dy) const;

    /**
     * Returns a copy of this Polygon rotated about the origin (0 ,0).
     * @param degrees The angle to rotate this polygon by.
     * Positive: counterclockwise, negative: clockwise.
     * @return
     */
    [[nodiscard]]
    Ellipse rotate(float degrees) const;

    /**
     * Returns a scaled copy of this polygon. A negative scale factor will
     *   reflect about the corresponding axis.
     * @param sx Horizontal scale factor
     * @param sy Vertical scale factor
     * @return
     */
    [[nodiscard]]
    Ellipse scale(float sx, float sy) const;

    template<typename T> requires Arithmetic<T>
    inline bool containsPoint(const Point_<T>&pt) const {
        return _containsPoint(pt.template cast<float>());
    }

private:
    static constexpr double pi = std::numbers::pi;

    bool _containsPoint(const Point2f &p) const ;

    Eigen::Affine2f t;

    Modulator mod;

    float width;
    float height;
    float angle;
    float cx;
    float cy;
};

namespace pxsort {

    template<>
    inline bool Ellipse::containsPoint(const Point2f &pt) const {
        return _containsPoint(pt);
    }

}

#endif //PXSORT_ELLIPSE_H
