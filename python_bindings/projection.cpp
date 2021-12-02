#include <pybind11/pybind11.h>

#include "PixelProjection.h"
#include "wrapper.h"

using namespace pxsort;
namespace proj = pxsort::projection;
namespace py = pybind11;

PixelProjectionWrapper wrapLinear(float pR, float pG, float pB, float bias) {
    cv::Vec3f p(pR, pG, pB);
    return {proj::linear(p, bias)};
}

PixelProjectionWrapper wrapChannel(Channel ch) {
    return {proj::channel(ch)};
}

void init_projection(py::module_ &m) {

    py::class_<PixelProjectionWrapper>
            (m, "PixelProjection")
            .def_static("linear", &wrapLinear,
                        "Returns a PixelProjection that projects input "
                        "pixels onto the unit interval using the given "
                        "linear projection.")
            .def_static("channel", &wrapChannel,
                        "Returns a PixelProjection that projects the "
                        "specified channel of input pixels.");
}