#ifndef PXSORT2_PIXELPREDICATE_H
#define PXSORT2_PIXELPREDICATE_H

#include "common.h"

/**
 * A PixelPredicate is a predicate that evaluates whether some property of
 *   a pixel holds.
 * A PixelPredicate f maps a Pixel p to the real numbers such that for the
 *   equivalent predicate F on p, the following equivalency holds:
 *     f(p) >= 0   <==>    F(p) == true
 *     f(p) <  0   <==>    F(p) == false
 */
class pxsort::PixelPredicate : public CloneableInterface<PixelPredicate> {
public:
    virtual float operator()(const Pixel&) = 0;

    template<typename T>
        requires is_ptr_to_derived<PixelPredicate, T>
    std::unique_ptr<PixelPredicate> logical_and(T &other) {
        return _conjunction(other->clone());
    }

    template<typename T>
        requires is_ptr_to_derived<PixelPredicate, T>
    std::unique_ptr<PixelPredicate> logical_or(T &other) {
        return _disjunction(other->clone());
    }

    std::unique_ptr<PixelPredicate> negate();

private:
    std::unique_ptr<PixelPredicate>
    _conjunction(std::unique_ptr<PixelPredicate> &);

    std::unique_ptr<PixelPredicate>
    _disjunction(std::unique_ptr<PixelPredicate> &);
};




#endif //PXSORT2_PIXELPREDICATE_H
