#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Predicate.h"

using namespace pxsort;
namespace pred = pxsort::predicate;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelPredicate);

PixelPredicate pyLinearProjectionPredicate(float pR, float pG,
                                           float pB, float bias) {
    cv::Vec3f p(pR, pG, pB);
    return pred::linear(p, bias);
}

void init_predicate(py::module_ &m) {

    m.def("predicate_linear", &pyLinearProjectionPredicate);
    m.def("predicate_threshold", &pred::threshold);

}

