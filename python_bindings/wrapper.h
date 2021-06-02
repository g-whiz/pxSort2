#ifndef PXSORT2_WRAPPER_H
#define PXSORT2_WRAPPER_H

#include "common.h"

struct PixelPredicateWrapper {
    pxsort::PixelPredicate predicate;
} __attribute__((aligned(32)));

struct PixelProjectionWrapper {
    pxsort::PixelProjection projection;
} __attribute__((aligned(32)));


#endif //PXSORT2_WRAPPER_H
