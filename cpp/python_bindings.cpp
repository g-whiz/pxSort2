#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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

PYBIND11_MODULE(pxsort, m) {
    // todo: complete doc
    m.doc() = R"pbdoc(
        PxSort Python bindings
        ----------------------

        .. currentmodule:: pxsort

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


Image createImageFromPyBuffer(const py::buffer& buf) {
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

void bindImage(py::module_ &m) {
    py::class_<Image>(m, "Image")
            .def(py::init<uint32_t, uint32_t, uint32_t>())
            .def(py::init(&createImageFromPyBuffer))
            .def_buffer([](Image &img) -> py::buffer_info {
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
            });
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

void bindSegmentPixels(py::module_ &m) {
    py::class_<SegmentPixels>(m, "SegmentPixels")
            .def("asdf_restriction", &SegmentPixels::asdfRestriction)
            .def("filter_restriction", &SegmentPixels::filterRestriction)
            .def("unrestricted", &SegmentPixels::unrestricted)
            .def("__deepcopy__", &SegmentPixels::deepCopy);
}

void bindSorter(py::module_ &m) {
    py::class_<Sorter>(m, "Sorter")
            .def_static("create_bucket_sorter", &Sorter::bucketSort)
            .def_static("create_heapify_sorter", &Sorter::heapify)
            .def_static("create_bubble_sorter", &Sorter::bubble)
            .def("__call__", &Sorter::operator());
}
