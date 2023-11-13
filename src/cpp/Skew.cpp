#include <Eigen/Geometry>

#include "Skew.h"
#include "util.h"

using namespace pxsort;
using namespace Eigen;


class Skew::SkewImpl {
public:
    virtual Point operator()(const Point &pt, int32_t chan) = 0;
    virtual ~SkewImpl() = default;
};

template<Skew::OutOfBoundsPolicy p>
inline int applyOOBPolicy(int x, int lo, int hi);

template<>
inline int applyOOBPolicy<pxsort::Skew::WRAP>(int x, int lo, int hi){
    return modulo((x - lo), (hi - lo)) + lo;
}

template<>
inline int applyOOBPolicy<pxsort::Skew::CLAMP>(int x, int lo, int hi){
    return clamp(x, lo, hi);
}

template<Skew::OutOfBoundsPolicy p>
inline Point applyOOBPolicy(const Point& x,
                            const Point& lo,
                            const Point& hi);

template<>
inline Point applyOOBPolicy<pxsort::Skew::WRAP>(const Point& x,
                                                const Point& lo,
                                                const Point& hi) {
    return modulo((x - lo), (hi - lo)) + lo;
}

template<>
inline Point applyOOBPolicy<pxsort::Skew::CLAMP>(const Point& x,
                                                 const Point& lo,
                                                 const Point& hi) {
    return x.constrain(lo, hi);
}

template<Skew::OutOfBoundsPolicy p>
struct ConstantSkew : public Skew::SkewImpl {
    explicit
    ConstantSkew(std::vector<Point> skews)
            : channelSkews(std::move(skews)) {}

    ~ConstantSkew() override = default;

private:
    std::vector<Point> channelSkews;

    Point operator()(const Point &pt, int chan) override {
        int trueChan = applyOOBPolicy<p>(chan, 0, channelSkews.size());
        return channelSkews[trueChan];
    }
};

template<Skew::OutOfBoundsPolicy p>
struct FunctionalSkew : public Skew::SkewImpl {
    ~FunctionalSkew() override = default;

    explicit
    FunctionalSkew(Skew::SkewFunction skew, int im_w, int im_h, int im_d)
            : skews(new Point[im_w * im_h * im_d]),
              width(im_w), height(im_h), depth(im_d) {
#ifdef PXSORT_DEBUG
        assert(im_w > 0);
        assert(im_h > 0);
        assert(im_d > 0);
#endif
        // Initialize skew table using the given skew function.
        #pragma omp parallel for collapse(3) default(none) shared(skew)
        for (int32_t x = 0; x < width; x++)
            for(int32_t y = 0; y < height; y++)
                for(int32_t ch = 0; ch < depth; ch++) {
                    const Point pt(x, y);
                    skew(pt.data(), ch, skewAt(x, y, ch).data());
                }
    }

private:
    int width;
    int height;
    int depth;
    std::unique_ptr<Point[]> skews;

    inline Point& skewAt(int x, int y, int chan) {
        return skews[chan + x * depth + y * width * depth];
    }

    Point operator()(const Point &pt, int chan) override {
        const int trueChan = applyOOBPolicy<p>(chan, 0, depth);
        const Point truePt = applyOOBPolicy<p>(pt, {0, 0}, {width, height});
        return skewAt(truePt.x(), truePt.y(), trueChan);
    }
};

struct SkewTransform : public Skew::SkewImpl {
    SkewTransform(const std::shared_ptr<Skew::SkewImpl> &skew,
                  const Affine2f &t) {
        // if given an instance of a SkewTransform, consolidate the transforms
        // to avoid unnecessary chain of function calls
        auto *child = dynamic_cast<SkewTransform *>(skew.get());
        if (child){
            this->skew = child->skew;
            this->t = t * child->t;
        } else {
            this->skew = skew;
            this->t = t;
        }
    }

    ~SkewTransform() override = default;

private:
    Point operator()(const Point &pt, int32_t chan) override {
        auto s = ((*skew)(pt, chan)).cast<float>();
        return (t * s).cast<int>();
    }

    std::shared_ptr<Skew::SkewImpl> skew;
    Affine2f t;
};

pxsort::Skew::Skew(SkewFunction skew,
                   int im_w, int im_h, int im_d,
                   Skew::OutOfBoundsPolicy p) {
    if (p == WRAP) {
        pImpl = std::make_shared<FunctionalSkew<WRAP>>(skew,
                                                       im_w, im_h, im_d);
    } else {
        pImpl = std::make_shared<FunctionalSkew<CLAMP>>(skew,
                                                       im_w, im_h, im_d);
    }
}

pxsort::Skew::Skew(const std::vector<Point> &skews,
                   Skew::OutOfBoundsPolicy p) {
    if (p == WRAP) {
        pImpl = std::make_shared<ConstantSkew<WRAP>>(skews);
    } else {
        pImpl = std::make_shared<ConstantSkew<CLAMP>>(skews);
    }
}

Point pxsort::Skew::operator()(const Point &pt, int32_t chan) {
    auto skew = (*pImpl)(pt, chan);
    return pt + skew;
}

pxsort::Skew::Skew() : Skew(std::vector<Point>{{0, 0}}) {}

Skew pxsort::Skew::scale(float sx, float sy) const {
    Affine2f const t = Affine2f::Identity() * Scaling(sx, sy);
    auto newPImpl = std::make_shared<SkewTransform>(pImpl, t);
    return Skew(newPImpl);
}

Skew pxsort::Skew::rotate(float degrees) const {
    float radians = (2 * std::numbers::pi) * (degrees / 360.0);
    Affine2f t = Affine2f::Identity() * Rotation2Df(radians);
    auto newPImpl = std::make_shared<SkewTransform>(pImpl, t);
    return Skew(newPImpl);
}

Skew pxsort::Skew::translate(int dx, int dy) const {
    Affine2f t = Affine2f::Identity() * Translation2f(dx, dy);
    auto newPImpl = std::make_shared<SkewTransform>(pImpl, t);
    return Skew(newPImpl);
}

pxsort::Skew::Skew(std::shared_ptr<SkewImpl> pImpl)
    : pImpl(std::move(pImpl)) {}
