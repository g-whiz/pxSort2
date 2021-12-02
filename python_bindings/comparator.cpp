#include <pybind11/pybind11.h>

#include "PixelComparator.h"

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

    py::class_<PixelComparator>
            (m, "PixelComparator")
    .def_static("channel", &cmp::channel,
          "Returns a PixelComparator that compares the specified channel from "
          "its two input pixels.")
    .def_static("linear", &pyLinearProjectionComparator,
          "Returns a PixelComparator that projects its two input pixels "
          "onto the real line using the given linear map, and compares "
          "the values of the pixels' images under the projection.");
}

