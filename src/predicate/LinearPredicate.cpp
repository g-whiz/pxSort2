#include "LinearPredicate.h"

using namespace pxsort;

float LinearPredicate::operator()(const Pixel &pixel) {
    return M.dot(pixel) + b;
}

LinearPredicate::LinearPredicate(const cv::Vec3f &M, float b) 
    : M(M), b(b){}

LinearPredicate::LinearPredicate(const LinearPredicate &other)
    : M(other.M), b(other.b) {}
