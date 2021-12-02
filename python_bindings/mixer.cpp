#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "PixelMixer.h"

using namespace pxsort;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelMixer);

PixelMixer pyLinearMixer(const py::array_t<float>& tArray) {
    auto T = std::make_shared<cv::Matx66f>();
    py::buffer_info info = tArray.request();

    if (info.ndim != 2)
        throw std::runtime_error("Incompatible dimension for given array Base "
                                 "- must be 2-dimensional.");

    if (info.shape[0] != 6 || info.shape[1] != 6)
        throw std::runtime_error("Incompatible shape for given array Base "
                                 "- must be (6, 6).");

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            (*T)(i, j) = tArray.at(i, j);

    return mixer::linear(T);
}

void init_mixer(py::module_ &m) {
    py::class_<PixelMixer>
            (m, "PixelMixer")
        .def_static("linear", &pyLinearMixer,
        "Returns a PixelMixer that applies the linear transformation defined "
        "by the given 6x6 matrix to input pixels. The two 3-channel input "
        "pixels are concatenated. And the transformation is applied to the "
        "resulting 6-dimensional vector.")
        .def_static("swapper", &mixer::swapper,
        "Returns a PixelMixer that swaps the channels specified by the given "
        "Swap.")
        .def_static("copier", &mixer::copier,
        "Returns a PixelMixer that copies channels from the input pixels to "
        "the output pixels as specified.");

    py::enum_<mixer::Swap>
            (m, "Swap")
            .value("RED", mixer::R, "Swap the red channels.")
            .value("GREEN", mixer::G, "Swap the green channels.")
            .value("BLUE", mixer::B, "Swap the blue channels.")
            .value("RG", mixer::RG, "Swap red and green channels.")
            .value("RB", mixer::RB, "Swap red and blue channels.")
            .value("GB", mixer::GB, "Swap green and blue channels.")
            .value("RGB", mixer::RGB, "Swap all channels.");

    py::enum_<mixer::InputChannel>
            (m, "InputChannel")
            .value("IN1_R", mixer::IN1_R,
                   "The red channel of the first input pixel.")
            .value("IN1_G", mixer::IN1_G,
                   "The green channel of the first input pixel.")
            .value("IN1_B", mixer::IN1_B,
                   "The blue channel of the first input pixel.")
            .value("IN2_R", mixer::IN2_R,
                   "The red channel of the second input pixel.")
            .value("IN2_G", mixer::IN2_G,
                   "The green channel of the second input pixel.")
            .value("IN2_B", mixer::IN2_B,
                   "The blue channel of the second input pixel.");
}

