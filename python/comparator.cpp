#include <pybind11/pybind11.h>

#include "Comparator.h"

using namespace pxsort;
namespace cmp = pxsort::comparator;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelComparator);

PixelComparator pyLinearProjectionComparator(float pR, float pG, float pB,
                                             cmp::Order order=cmp::DESCENDING) {
    cv::Vec3f p(pR, pG, pB);
    return cmp::linear(p, order);
}


void init_comparator(py::module_ &m) {

    py::enum_<cmp::Order>
            (m, "Order", "The order to use when sorting pixels.")
            .value("Ascending", cmp::ASCENDING)
            .value("Descending", cmp::DESCENDING);

    m.def("comparator_channel", &cmp::channel);
    m.def("comparator_linear_projection", &pyLinearProjectionComparator);
}

