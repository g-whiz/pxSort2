#include "Polygon.h"
#include "util.h"

using namespace pxsort;
using namespace Eigen;

/* Returns true if the edge between verts a and b crosses an infinite
 * horizontal ray cast to the left of point px. */
inline float leftTest (const Point2f &p, const Point2f &a, const Point2f &b) {
    return ((b.x() - a.x()) * (p.y() - a.y())
          - (p.x() - a.x()) * (b.y() - a.y()));
}

/* Winding-number based point-in-polygon test. Linear in # of vertices.*/
bool pxsort::Polygon::_containsPoint(const Point2f &p) const {
    int windingNumber = 0;

    for (int edge = 0; edge < verts.size(); edge++) {
        const auto& a = verts[edge];
        const auto& b = verts[(edge + 1) % verts.size()];


        if(a.y() <= p.y()) {
            if (b.y() > p.y() && leftTest(p, a, b) > 0)
                windingNumber++;
        }
        else {
            if (b.y() <= p.y() && leftTest(p, a, b) < 0)
                windingNumber--;
        }
    }

    return windingNumber != 0;
}

Polygon::Polygon(int sides, float radius, float cX, float cY) {
    sides = max(sides, 3);
    verts = std::vector<Point2f>(sides);

    radius = radius == 0.0 ? 1.0 : abs(radius);


    const double angleIncrement = (2.0 * pi) / static_cast<float>(sides);

    const Point2f center(cX, cY);
    const Point2f vertex(0, 1);

    // 1st vertex is always vertical -> horizontal symmetry
    verts[0] = (vertex * radius) + center;

    // compute remaining vertices: rotate, scale, then translate.
    for(int i = 1; i < sides; i++){
        const Rotation2D<float> rot(angleIncrement * static_cast<double>(i));
        verts[i] = ((rot * vertex) * radius) + center;
    }
}

Polygon Polygon::shear(float sx, float sy) {
    auto t = AffineCompact2f::Identity();
    t.shear(sx, sy);

    return this->transformed(t);
}

Polygon Polygon::translate(float dx, float dy) const {
    auto t = AffineCompact2f::Identity();
    t.translate(Point2f(dx, dy));

    return this->transformed(t);
}

Polygon Polygon::rotate(float degrees) const {
    const double radians = (2.0 * pi) * (degrees / 360.0);
    auto t = AffineCompact2f::Identity();
    t.rotate(radians);

    return this->transformed(t);
}

Polygon Polygon::scale(float sx, float sy) const {
    auto t = AffineCompact2f::Identity();
    t.scale(Point2f(sx, sy));

    return this->transformed(t);
}

Polygon::Polygon(std::vector<Point2f> vertices) : verts(std::move(vertices)) {}

Polygon Polygon::transformed(const AffineCompact2f &t) const {
    std::vector<Point2f> transformedVerts(verts.size());
    for (int i = 0; i < verts.size(); i++) {
        transformedVerts[i] = t * verts[i];
    }

    return {transformedVerts};
}

Polygon::Polygon(const std::vector<std::pair<float, float>> &vertices)
    : verts(vertices.size()){
    for (int i = 0; i < verts.size(); i++) {
        auto &[x, y] = vertices[i];
        verts[i] = {x, y};
    }
}

Polygon Polygon::modulate(Polygon::Modulator m, int vertices) const {
    Point2f centroid;
    for (int i = 0; i < verts.size(); i++) {
        centroid += verts[i];
    }
    centroid /= verts.size();

    int const nVerts = max(vertices, static_cast<int>(verts.size()));
    std::vector<Point2f> modVerts(nVerts);

    float const vertRatio = static_cast<float>(nVerts)
                          / static_cast<float>(verts.size());
    #pragma omp parallel for default(none) \
            shared(m, nVerts, vertRatio, centroid, modVerts)
    for (int v = 0; v < nVerts; v++) {
        int edge = min(static_cast<float>(verts.size() - 1),
                       static_cast<float>(v) / vertRatio);
        Point2f const &a = verts[edge];
        Point2f const &b = verts[(edge + 1) % verts.size()];

        float const t = std::fmod(static_cast<float>(v), vertRatio) / vertRatio;
        auto c = t * b + (1 - t) * a;
        auto d = c - centroid;

        float const phi = std::atan2(d.x(), d.y());
        auto e = (d * m(phi)) + centroid;
        modVerts[v] = e;
    }

    return Polygon(modVerts);
}
