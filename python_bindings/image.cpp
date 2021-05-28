#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Image.h"

namespace py = pybind11;

using namespace pxsort;

enum ExceptionType {
    BAD_NDIMS,
    BAD_NCHANNELS,
    BAD_DATA_TYPE
};

class DataFormatException : std::exception {
public:
    const ExceptionType type;
    explicit DataFormatException(ExceptionType type) : type(type) {}

    [[nodiscard]] const char*
    what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
        switch (type) {
            case BAD_NDIMS:
                return "Incorrect shape: the given buffer must have "
                       "exactly 3 dimensions, with shape (height, width, 4).";
            case BAD_NCHANNELS:
                return "Incorrect number of channels: the given buffer must "
                       "have exactly 4 channels, corresponding to the RGB32 "
                       "color model.";
            case BAD_DATA_TYPE:
                return "Incorrect data type: the given buffer must consist of "
                       "8-bit integers, corresponding to the RGB32 color model.";
            default:
                return "Unknown error: Invalid ExceptionType received.";
        }
    }
};

/* Wrapper to construct an Image from a RGB32 numpy ndarray */
static std::shared_ptr<Image> makeImage(py::buffer &data,
                                        Image::ColorSpace cs)
{
    py::buffer_info info = data.request();
    if (info.ndim != 3) {
        throw DataFormatException(BAD_NDIMS);
    }
    if (info.itemsize != 1) {
        throw DataFormatException(BAD_DATA_TYPE);
    }
    if (info.shape[2] != 4) {
        throw DataFormatException(BAD_NCHANNELS);
    }

    int w = info.shape[1];
    int h = info.shape[0];
    auto data_ptr = (uint8_t *) info.ptr;
    return std::make_shared<Image>(w, h, cs, data_ptr);
}

/* Returns a copy of an Image's stored data as an ndarray. */
py::array getData(const std::shared_ptr<Image>& thisImage){
    std::unique_ptr<uint8_t[]> data = thisImage->toRGB32();
    int h = thisImage->height;
    int w = thisImage->width;

    py::dtype dt(py::format_descriptor<uint8_t>::format());
    py::array::ShapeContainer shape({h, w, 4});
    py::array::StridesContainer strides({w * 4, 4, 1});

    return py::array(dt, shape, strides, data.release());
}


void init_image(py::module_ &m) {
    py::register_exception<DataFormatException>(m, "DataFormatException");

    py::enum_<Image::ColorSpace>(m, "ColorSpace",
                                 "Color spaces supported by the Image class.")
            .value("RGB", Image::RGB)
            .value("XYZ", Image::XYZ)
            .value("Lab", Image::Lab)
            .value("Luv", Image::Luv)
            .value("YCrCb", Image::YCrCb)
            .value("HSV", Image::HSV)
            .value("HLS", Image::HLS)
            .export_values();

    py::class_<Image, std::shared_ptr<Image>>(m, "Image", "An Image.")
            .def(py::init(&makeImage))
            .def("to_array", &getData,
                 "Returns a numpy array containing a copy of this Image's data "
                 "in the RGB32 color format.")
            .def_readonly("width", &Image::width)
            .def_readonly("height", &Image::height)
            .def_readonly("color_space", &Image::colorSpace);
}