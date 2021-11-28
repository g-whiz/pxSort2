#include "ThresholdPredicate.h"

using namespace pxsort;

float ThresholdPredicate::operator()(const Pixel &pixel) {
    float rLo = pixel[RED] - rMin;
    float rHi = rMax - pixel[RED];
    float gLo = pixel[GREEN] - gMin;
    float gHi = gMax - pixel[GREEN];
    float bLo = pixel[BLUE] - bMin;
    float bHi = bMax - pixel[BLUE];

    return min(rLo, rHi, gLo, gHi, bLo, bHi);
}

ThresholdPredicate::ThresholdPredicate(float rMin, float rMax,
                                       float gMin, float gMax,
                                       float bMin, float bMax)
    : rMin(rMin), rMax(rMax), gMin(gMin), gMax(gMax), bMin(bMin), bMax(bMax) {}

ThresholdPredicate::ThresholdPredicate(const ThresholdPredicate &other)
    : rMin(other.rMin), rMax(other.rMax), gMin(other.gMin),
      gMax(other.gMax), bMin(other.bMin), bMax(other.bMax) {}
