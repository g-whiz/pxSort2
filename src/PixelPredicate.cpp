#include "PixelPredicate.h"

using namespace pxsort;

template<typename Impl>
class Product;
class Conjunction;
class Disjunction;
class Negation;


template<typename Impl>
class Product : public CloneableImpl<PixelPredicate, Impl> {
public:
    Product(std::unique_ptr<PixelPredicate> a,
            std::unique_ptr<PixelPredicate> b)
              : a(std::move(a)), b(std::move(b)){}

    Product(const Product &other)
        : Product(other.a->clone(), other.b->clone()){}

protected:
    std::unique_ptr<PixelPredicate> a;
    std::unique_ptr<PixelPredicate> b;
};

class Conjunction : public Product<Conjunction> {
public:
    using Product<Conjunction>::Product;

private:
    float operator()(const Pixel &pixel) override {
        return MIN((*a)(pixel), (*b)(pixel));
    }
};

class Disjunction : public Product<Disjunction> {
public:
    using Product<Disjunction>::Product;

private:
    float operator()(const Pixel &pixel) override {
        return MAX((*a)(pixel), (*b)(pixel));
    }
};

class Negation : public CloneableImpl<PixelPredicate, Negation> {
public:
    explicit Negation(std::unique_ptr<PixelPredicate> a) : a(std::move(a)) {}

    Negation(const Negation &other) : a(other.a->clone()){}

private:
    float operator()(const Pixel &pixel) override {
        return -(*a)(pixel);
    }

    std::unique_ptr<PixelPredicate> a;
};

std::unique_ptr<PixelPredicate> PixelPredicate::negate() {
    return std::unique_ptr<PixelPredicate>(new Negation(this->clone()));
}

std::unique_ptr<PixelPredicate>
        PixelPredicate::_conjunction(std::unique_ptr<PixelPredicate> &other) {
    return std::unique_ptr<PixelPredicate>(
            new Conjunction(this->clone(), std::move(other)));
}

std::unique_ptr<PixelPredicate>
        PixelPredicate::_disjunction(std::unique_ptr<PixelPredicate> &other) {
    return std::unique_ptr<PixelPredicate>(
            new Disjunction(this->clone(), std::move(other)));
}
