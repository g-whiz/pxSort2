#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PixelPredicate.h"
#include "wrapper.h"

using namespace pxsort;
namespace pred = pxsort::predicate;
namespace py = pybind11;

PixelPredicateWrapper pyLinearProjectionPredicate(float pR, float pG,
                                           float pB, float bias) {
    cv::Vec3f p(pR, pG, pB);
    return {pred::linear(p, bias)};
}

PixelPredicateWrapper wrapThreshold(std::optional<float> rMin={},
                                    std::optional<float> rMax={},
                                    std::optional<float> gMin={},
                                    std::optional<float> gMax={},
                                    std::optional<float> bMin={},
                                    std::optional<float> bMax={}) {
    return {pred::threshold(rMin, rMax, gMin, gMax, bMin, bMax)};
}

void init_predicate(py::module_ &m) {

    py::class_<PixelPredicateWrapper>
            (m, "PixelPredicate")
    .def_static("linear", &pyLinearProjectionPredicate,
          "Returns a PixelPredicate that projects its input pixel "
          "onto the real line using the given linear map such that "
          "the result is \"true\" when the image of the pixel under the "
          "projection is nonnegative.")
    .def_static("threshold", &wrapThreshold,
          "Returns a PixelPredicate that is \"true\" for a given pixel if the "
          "values of the pixel's channels are within the specified thresholds.",
          py::arg("r_min") = std::nullopt, py::arg("r_max") = std::nullopt,
          py::arg("g_min") = std::nullopt, py::arg("g_max") = std::nullopt,
          py::arg("b_min") = std::nullopt, py::arg("b_max") = std::nullopt);
}

