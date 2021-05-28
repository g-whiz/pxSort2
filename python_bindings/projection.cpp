#include <pybind11/pybind11.h>

#include "Projection.h"

using namespace pxsort;
namespace proj = pxsort::projection;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelProjection);

PixelProjection pyLinearProjection(float pR, float pG, float pB, float bias) {
    cv::Vec3f p(pR, pG, pB);
    return proj::linear(p, bias);
}

void init_projection(py::module_ &m) {

    m.def("projection_linear", &pyLinearProjection);
    m.def("projection_channel", &proj::channel);

}