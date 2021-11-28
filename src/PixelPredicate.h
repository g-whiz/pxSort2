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
    virtual ~PixelPredicate() = default;

    template<template<typename> class Pointer>
    std::unique_ptr<PixelPredicate> operator&&(
            Pointer<PixelPredicate> &other) {
        return conjunction(other->clone());
    }

    template<template<typename> class Pointer>
    std::unique_ptr<PixelPredicate> operator||(
            Pointer<PixelPredicate> &other) {
        return disjunction(other->clone());
    }

    std::unique_ptr<PixelPredicate> operator! ();

private:
    std::unique_ptr<PixelPredicate>
    conjunction(std::unique_ptr<PixelPredicate> &);

    std::unique_ptr<PixelPredicate>
    disjunction(std::unique_ptr<PixelPredicate> &);
};




#endif //PXSORT2_PIXELPREDICATE_H
