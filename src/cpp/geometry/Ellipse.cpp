#include "Ellipse.h"

#include <utility>
#include "Modulation.h"

using namespace pxsort;
using namespace Eigen;

Ellipse::Ellipse(float radius, float cx, float cy)
    : Ellipse(radius, radius, 0, cx, cy) {}



Ellipse::Ellipse(float w, float h, float angle, float cx, float cy)
    : mod(modulator::identity()),
      height(h > 0 ? h : 1.0),
      width(w > 0 ? w : 1.0)
{
    const float radians = (2.0 * pi) * (angle / 360.0);

    // t: inverse of the affine transform mapping the unit circle to the given
    //    ellipse.
    t = Scaling(Vector2f(2 / width, 2 / height))
            * Rotation2Df(-radians)
            * Translation2f(-cx, -cy);
}

bool Ellipse::_containsPoint(const Point2f &p) const {
    Point2f const q = t * p;
    float const r = std::hypot(q.x(), q.y());
    float const phi = std::atan2(q.y(), q.x());

    return r <= mod(phi);
}

Ellipse Ellipse::translate(float dx, float dy) const {
    Ellipse e{width, height, angle, cx + dx, cy + dy};
    e.mod = mod;
    return e;
}

Ellipse Ellipse::rotate(float degrees) const {
    Ellipse e{width, height, angle + degrees, cx, cy};
    e.mod = mod;
    return e;
}

Ellipse Ellipse::scale(float sx, float sy) const {
    Ellipse e{width * sx, height * sy, angle, cx, cy};
    e.mod = mod;
    return e;
}

Ellipse Ellipse::modulate(Ellipse::Modulator m) const {
    Ellipse e = *this;
    e.mod = std::move(m);
    return e;
}
