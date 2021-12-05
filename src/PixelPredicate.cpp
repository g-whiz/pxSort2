#include "PixelPredicate.h"

using namespace pxsort;

class PixelPredicate::Impl : public CloneableInterface<Impl> {
public:
    virtual float operator()(const Pixel&) const = 0;
};


class PixelPredicate::LinearImpl :
        public CloneableImpl<Impl, LinearImpl> {
    const cv::Vec3f M;
    const float b;

public:
    LinearImpl(const cv::Vec3f& M, float b) : M(M), b(b) {}

    LinearImpl(const LinearImpl &other) : M(other.M), b(other.b) {}

private:
    float operator()(const Pixel &pixel) const override {
        return M.dot(pixel) + b;
    }
};

class PixelPredicate::ThresholdImpl :
        public CloneableImpl<Impl, ThresholdImpl> {
    const float rMin;
    const float rMax;
    const float gMin;
    const float gMax;
    const float bMin;
    const float bMax;

public:
    explicit ThresholdImpl(float rMin = 0.0, float rMax = 1.0,
                           float gMin = 0.0, float gMax = 1.0,
                           float bMin = 0.0, float bMax = 1.0)
         : rMin(rMin), rMax(rMax),
           gMin(gMin), gMax(gMax),
           bMin(bMin), bMax(bMax) {}

    ThresholdImpl(const ThresholdImpl &other)
        : rMin(other.rMin), rMax(other.rMax),
          gMin(other.gMin), gMax(other.gMax),
          bMin(other.bMin), bMax(other.bMax) {}

private:
    float operator()(const Pixel &pixel) const override {
        float rLo = pixel[RED] - rMin;
        float rHi = rMax - pixel[RED];
        float gLo = pixel[GREEN] - gMin;
        float gHi = gMax - pixel[GREEN];
        float bLo = pixel[BLUE] - bMin;
        float bHi = bMax - pixel[BLUE];

        return min(rLo, rHi, gLo, gHi, bLo, bHi);
    }
};


class PixelPredicate::CombinationImpl :
        public CloneableImpl<Impl, CombinationImpl> {
public:
    enum Operand {
        AND,
        OR
    };

private:
    const Operand op;
    const PixelPredicate a;
    const PixelPredicate b;

public:
    CombinationImpl(Operand op,
                    const PixelPredicate &a,
                    const PixelPredicate &b)
              : op(op), a(a), b(b) {}

    CombinationImpl(const CombinationImpl &other)
        : CombinationImpl(other.op, other.a, other.b){}

private:
    float operator()(const Pixel &pixel) const override {
        switch (op) {
            case AND:
                return MIN(a(pixel), b(pixel));
            case OR:
                return MAX(a(pixel), b(pixel));
        }
    }
};

class PixelPredicate::NegationImpl :
        public CloneableImpl<Impl, NegationImpl> {
    PixelPredicate a;

public:
    explicit NegationImpl(const PixelPredicate &a) : a(a) {}

    NegationImpl(const NegationImpl &other) : a(other.a) {}

private:
    float operator()(const Pixel &pixel) const override {
        return -a(pixel);
    }
};

PixelPredicate::PixelPredicate(float rMin, float rMax,
                               float gMin, float gMax,
                               float bMin, float bMax)
    : pImpl(new ThresholdImpl(rMin, rMax, gMin, gMax, bMin, bMax)) {}

PixelPredicate::PixelPredicate(const cv::Vec3f &proj, float bias)
    : pImpl(new LinearImpl(proj, bias)) {}

PixelPredicate::PixelPredicate(const PixelPredicate &other)
    : pImpl(other.pImpl->clone()) {}

float PixelPredicate::operator()(const Pixel &pixel) const {
    return (*pImpl)(pixel);
}

PixelPredicate PixelPredicate::operator&&(const PixelPredicate &that) const {
    return PixelPredicate(
            new CombinationImpl(CombinationImpl::AND, *this, that));
}

PixelPredicate PixelPredicate::operator||(const PixelPredicate &that) const {
    return PixelPredicate(
            new CombinationImpl(CombinationImpl::OR, *this, that));
}

PixelPredicate PixelPredicate::operator!() const {
    return PixelPredicate(new NegationImpl(*this));
}

PixelPredicate::PixelPredicate(PixelPredicate::Impl *pImpl)
    : pImpl(pImpl){}
