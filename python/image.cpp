#include <boost/python/module.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/numpy/ndarray.hpp>

#include "Image.h"

using namespace boost::python;
namespace np = boost::python::numpy;

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
                return "Incorrect shape: the given ndarray must have "
                       "exactly 3 dimensions.";
            case BAD_NCHANNELS:
                return "Incorrect number of channels: the given ndarray must "
                       "have exactly 4 channels, corresponding to the RGB32 "
                       "color model.";
            case BAD_DATA_TYPE:
                return "Incorrect dtype: the given ndarray must have a dtype "
                       "of 'uint8_t', corresponding to the RGB32 color model.";
            default:
                return "Unknown error: Invalid ExceptionType received.";
        }
    }
};

/* Error translator to print DataFormatException messages thrown by makeImage.
 */
static void translate(DataFormatException const& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

/* Wrapper to construct an Image from a RGB32 numpy ndarray */
static boost::shared_ptr<Image> makeImage(np::ndarray &data,
                                          Image::ColorSpace cs)
{
    if (data.get_nd() != 3) {
        throw DataFormatException(BAD_NDIMS);
    }
    if (data.get_dtype() != np::dtype::get_builtin<uint8_t>()) {
        throw DataFormatException(BAD_DATA_TYPE);
    }
    if (data.shape(2) != 4) {
        throw DataFormatException(BAD_NCHANNELS);
    }

    int w = data.shape(1);
    int h = data.shape(0);
    auto *data_ptr = (uint8_t *) data.get_data();
    return boost::shared_ptr<Image>(new Image(w, h, cs, data_ptr));
}

/* Returns a copy of an Image's stored data as an ndarray. */
np::ndarray getData(Image *thisImage){
    std::unique_ptr<uint8_t[]> data = thisImage->toRGB32();
    int h = thisImage->height;
    int w = thisImage->width;

    np::dtype dt = np::dtype::get_builtin<uint8_t>();
    tuple shape = make_tuple(h, w, 4);
    tuple strides = make_tuple(w * 4, 4, 1);

    return np::from_data(data.release(), dt, shape, strides, object());
}


BOOST_PYTHON_MODULE(image)
{
    register_exception_translator<DataFormatException>(&translate);

    enum_<Image::ColorSpace>("ColorSpace",
                             "Color spaces supported by the Image class.")
            .value("RGB", Image::RGB)
            .value("XYZ", Image::XYZ)
            .value("Lab", Image::Lab)
            .value("Luv", Image::Luv)
            .value("YCrCb", Image::YCrCb)
            .value("HSV", Image::HSV)
            .value("HLS", Image::HLS)
            .export_values();

    class_<Image>("Image", "An Image.", no_init)
            .def("__init__", make_constructor(&makeImage))
            .def("to_array", &getData,
                 "Returns a numpy array containing a copy of this Image's data "
                 "in the RGB32 color format.")
            .def_readonly("width", &Image::width)
            .def_readonly("height", &Image::height)
            .def_readonly("color_space", &Image::colorSpace);
}