#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "Image.h"
#include "Map.h"
#include "Segment.h"
#include "Sorter.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace pxsort;

void bindImage(py::module_ &m);
void bindMap(py::module_ &m);
void bindSegment(py::module_ &m);
void bindSegmentPixels(py::module_ &m);
void bindSorter(py::module_ &m);

PYBIND11_MODULE(_native, m) {
    // todo: complete doc
    m.doc() = R"pbdoc(
        PxSort Python bindings
        ----------------------

        .. currentmodule:: _pxsort

        ..autosummary::
            :toctree: _generate


    )pbdoc";

    bindImage(m);
    bindMap(m);
    bindSegment(m);
    bindSegmentPixels(m);
    bindSorter(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}


Image pyBufferToImage(const py::buffer& buf) {
    /* Request a buffer descriptor from Python */
    py::buffer_info info = buf.request();

    /* Some basic validation checks ... */
    if (info.format != py::format_descriptor<float>::format())
        throw std::runtime_error("Incompatible format: expected a "
                                 "single-precision float array!");

    if (info.ndim != 3)
        throw std::runtime_error("Incompatible buffer dimension: requires a 3d "
                                 "array.");

    if (info.shape[0] > IMAGE_MAX_WIDTH)
        throw std::runtime_error("Incompatible buffer shape: maximum width is "
                                 MACRO_STRINGIFY(IMAGE_MAX_WIDTH) ".");
    if (info.shape[1] > IMAGE_MAX_HEIGHT)
        throw std::runtime_error("Incompatible buffer shape: maximum height is "
                                 MACRO_STRINGIFY(IMAGE_MAX_HEIGHT) ".");
    if (info.shape[2] > IMAGE_MAX_DEPTH)
        throw std::runtime_error("Incompatible buffer shape: pixels may not "
                                 "have more than "
                                 MACRO_STRINGIFY(IMAGE_MAX_DEPTH) " channels.");

    uint32_t width = info.shape[0];
    uint32_t height = info.shape[1];
    uint32_t depth = info.shape[2];

    uint32_t colStride = info.strides[0] / sizeof(float);
    uint32_t rowStride = info.strides[1] / sizeof(float);
    uint32_t cnStride = info.strides[2] / sizeof(float);

    // Optimization: special case where buf already has the correct memory
    // layout. This constructor copies over image data in a CPU-friendly way
    // (i.e. minimizing cache-misses).
    if (cnStride == 1 && colStride == depth && rowStride == width * depth)
        return {width, height, depth, static_cast<float *>(info.ptr)};

    else {
        Image img(width, height, depth);
        auto data = static_cast<float *>(info.ptr);
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                for (int cn = 0; cn < depth; cn++) {
                    img.ptr(x, y)[cn] =
                            data[x * colStride + y * rowStride + cn * cnStride];
                }

        return img;
    }
}

py::buffer_info imageBuffer(Image &img) {
    return {
        img.ptr(0, 0),
        sizeof(float),
        py::format_descriptor<float>::format(),
        3,
        {img.width, img.height, img.depth},
        {sizeof(float) * img.depth,
         sizeof(float) * img.width * img.depth,
         sizeof(float)}
    };
}

void bindImage(py::module_ &m) {
    py::class_<Image>(m, "Image", py::buffer_protocol())
            .def(py::init<uint32_t, uint32_t, uint32_t>())
            .def(py::init(&pyBufferToImage))
            .def("__getitem__", &Image::at)
            .def_buffer(&imageBuffer)
            .def("shape",
                 [](Image &img) -> std::tuple<uint32_t, uint32_t, uint32_t> {
                return {img.width, img.height, img.depth};
            })
            .def("__array__",
                 [](Image &img) {return py::array(imageBuffer(img));})
            .def("A",
                 [](Image &img) {return py::array(imageBuffer(img));});
}

void bindMap(py::module_ &m) {
    py::class_<Map>(m, "Map")
            .def(py::init([](uint64_t f_ptr, uint32_t in_dim, uint32_t out_dim)
            {
                return Map(reinterpret_cast<Map::FuncPtr>(f_ptr),
                           in_dim, out_dim);
            }))
            .def_readonly("in_dim", &Map::inDim)
            .def_readonly("out_dim", &Map::outDim)
            .def("__lshift__", &Map::operator<<)
            .def("__or__", &Map::operator|)
            .def("__pow__", &Map::operator^)
            .def("__getitem__", &Map::operator[])
            .def("__call__", [](const Map &m, const std::vector<float> &x) {
                return m(x);
            })
            .def("__eq__", &Map::operator==)
            .def_static("concatenate", &Map::concatenate)
            .def_static("constant", &Map::constant);
}

void bindSegment(py::module_ &m) {
    py::enum_<Segment::Traversal>(m, "SegmentTraversal",
                                  "Traversal options for a Segment's pixels.")
            .value("Forward", Segment::FORWARD)
            .value("Reverse", Segment::REVERSE)
            .value("BinaryTreeBreadthFirst",
                   Segment::BINARY_TREE_BREADTH_FIRST);

    py::class_<Segment>(m, "Segment")
            .def(py::init<uint32_t, uint32_t, uint32_t, uint32_t>())
            .def(py::init<std::vector<Segment::Coordinates>>())
            .def("get_coordinates", &Segment::getCoordinates)
            .def("get_pixels", &Segment::getPixels)
            .def("put_pixels", &Segment::putPixels)
            .def("__sub__", &Segment::operator-)
            .def("__and__", &Segment::operator&)
            .def("__or__", &Segment::operator|)
            .def("sort_coordinates", &Segment::sorted)
            .def("filter_coordinates", &Segment::filter)
            .def("size", &Segment::size)
            .def("__getitem__", &Segment::operator[]);
}


SegmentPixels pyBufferToSegmentPixels(const py::buffer& buf) {
    /* Request a buffer descriptor from Python */
    py::buffer_info info = buf.request();

    /* Some basic validation checks ... */
    if (info.format != py::format_descriptor<float>::format())
        throw std::runtime_error("Incompatible format: expected a "
                                 "single-precision float array!");

    if (info.ndim != 2)
        throw std::runtime_error("Incompatible buffer dimension: requires a 2d "
                                 "array (i.e. an array of pixels).");

    if (info.shape[1] > IMAGE_MAX_DEPTH)
        throw std::runtime_error("Incompatible buffer shape: pixels may not "
                                 "have more than "
                                 MACRO_STRINGIFY(IMAGE_MAX_DEPTH) " channels.");

    auto len = info.shape[0];
    auto depth = info.shape[1];

    auto pxStride = info.strides[0] / sizeof(float);
    auto cnStride = info.strides[1] / sizeof(float);

    SegmentPixels segPx(len, depth);
    auto data = static_cast<float *>(info.ptr);
    for(int i = 0; i < len; i++)
        for (int cn = 0; cn < depth; cn++)
            segPx.at(i)[cn] = data[i * pxStride + cn * cnStride];

    return segPx;
}

py::buffer_info segmentPixelsBuffer(const SegmentPixels &segPx) {
    auto spu = segPx.unrestricted();
    return {
            spu.at(0),
            sizeof(float),
            py::format_descriptor<float>::format(),
            2,
            {spu.size(), spu.pixelDepth},
            {sizeof(float) * spu.pixelDepth,
             sizeof(float)}
    };
}

void bindSegmentPixels(py::module_ &m) {
    py::class_<SegmentPixels>(m, "SegmentPixels", py::buffer_protocol())
            .def(py::init(&pyBufferToSegmentPixels))
            .def("asdf_restriction", &SegmentPixels::asdfRestriction)
            .def("filter_restriction", &SegmentPixels::filterRestriction)
            .def("unrestricted", &SegmentPixels::unrestricted)
            .def("__deepcopy__", &SegmentPixels::deepCopy)
            .def_buffer(&segmentPixelsBuffer)
            .def("__array__",[](SegmentPixels &sp){
                return py::array(segmentPixelsBuffer(sp));});
}

void bindSorter(py::module_ &m) {
    py::class_<Sorter>(m, "Sorter")
            .def_static("create_bucket_sorter", &Sorter::bucketSort)
            .def_static("create_heapify_sorter", &Sorter::heapify)
            .def_static("create_bubble_sorter", &Sorter::bubble)
            .def("__call__", &Sorter::operator());
}
