
#include "PixelPredicate.h"

using namespace pxsort;

PixelPredicate predicate::linear(const cv::Vec3f& M, float b) {
    return [=](const Pixel &p) {
        return M.dot(p) + b;
    };
}

PixelPredicate
predicate::threshold(std::optional<float> rMin, std::optional<float> rMax,
                     std::optional<float> gMin, std::optional<float> gMax,
                     std::optional<float> bMin, std::optional<float> bMax) {
    /* For each defined threshold, add a clause checking that specific
     * threshold. */
    std::vector<PixelPredicate> clauses;
    float sign = 1.0;
    int channel = 0;
    for (const auto& maybeThresh : {rMin, rMax, gMin, gMax, bMin, bMax}) {
        if (maybeThresh) {
            clauses.emplace_back([=](const Pixel &p){
                return sign * (p[channel / 2] - maybeThresh.value());
            });
        }
        sign = -sign;
        channel++;
    }

    /* Return a predicate that returns the logical AND of the defined clauses.
     * If no clauses are defined, this returns a Pixel predicate that always
     * evaluates to true. */
    return [=](const Pixel &p){
        float result = 1.0;
        for (const auto& clause : clauses)
            result = MIN(result, clause(p));
        return result;
    };
}
