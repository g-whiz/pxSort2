#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "Image.h"
#include "Map.h"
#include "Segment.h"
#include "Sorter.h"
#include "geometry/Modulation.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace pxsort;

void bindImage(py::module_ &m);
void bindMap(py::module_ &m);
void bindSegment(py::module_ &m);
void bindSegmentPixels(py::module_ &m);
void bindSorter(py::module_ &m);
void bindEllipse(py::module_ &m);
void bindPolygon(py::module_ &m);
void bindSkew(py::module_ &m);
void bindModulator(py::module_ &m);

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
    bindEllipse(m);
    bindPolygon(m);
    bindSkew(m);
    bindModulator(m);

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

    const int32_t width = info.shape[0];
    const int32_t height = info.shape[1];
    const int32_t depth = info.shape[2];

    const int32_t colStride = info.strides[0] / sizeof(float);
    const int32_t rowStride = info.strides[1] / sizeof(float);
    const int32_t cnStride = info.strides[2] / sizeof(float);

    auto src_data = static_cast<float *>(info.ptr);
    std::shared_ptr<float[]> const dst_data(new float[width * height * depth]);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            #pragma omp simd
            for (int cn = 0; cn < depth; cn++) {
                dst_data[x * depth + y * width * depth + cn]
                    = src_data[x * colStride + y * rowStride + cn * cnStride];
            }

    Image img(width, height, depth, dst_data);
    return img;
}

py::buffer_info imageBuffer(Image &img) {
    return {
        img.ptr(0, img.height - 1),
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
    py::enum_<Image::Topology>(m, "ImageTopology",
        "Defines behaviour for out-of-bounds access of Image pixels.")
        .value("Square", Image::SQUARE)
        .value("Torus", Image::TORUS);

    py::class_<Image>(m, "Image", py::buffer_protocol())
            .def(py::init<uint32_t, uint32_t, uint32_t>())
            .def(py::init(&pyBufferToImage))
            .def("__getitem__", [](Image &img, std::tuple<int, int, int> t) {
                return img.at(get<0>(t), get<1>(t), get<2>(t));
            })
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
                return Map(reinterpret_cast<Map::fp_t>(f_ptr),
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
            .def(py::init<std::vector<std::pair<int, int>>>())
            .def("get_pixels", &Segment::getPixels,
                 py::call_guard<py::gil_scoped_release>())
            .def("put_pixels", &Segment::putPixels,
                 py::call_guard<py::gil_scoped_release>())
            .def("__sub__", &Segment::operator-)
            .def("__and__", &Segment::operator&)
            .def("__or__", &Segment::operator|)
            .def("sort_with_function", [](const Segment &s, uint64_t f_ptr) {
                     Segment::CoordinateProjection const cp
                         = reinterpret_cast<Segment::CoordinateProjPtr>(f_ptr);
                     return s.sorted(cp);
                 },
                 py::call_guard<py::gil_scoped_release>())
            .def("sort_along_angle", [](const Segment &s, float degrees) {
                     return s.sorted(degrees);
                 },
                 py::call_guard<py::gil_scoped_release>())
            .def("masked", [](const Segment &s, uint64_t f_ptr) {
                    Segment::CoordinateProjection const cp
                        = reinterpret_cast<Segment::CoordinateProjPtr>(f_ptr);
                    return s.filter(cp);
                },
                 py::call_guard<py::gil_scoped_release>())
            .def("masked",
                 [](const Segment& s, const Polygon& p) { return s.mask(p); },
                 py::call_guard<py::gil_scoped_release>())
            .def("masked",
                 [](const Segment& s, const Ellipse& e) { return s.mask(e); },
                 py::call_guard<py::gil_scoped_release>())
            .def("function_partition",
                 [](const Segment &s, uint64_t f_ptr, int n) {
                     Segment::CoordinateProjection const cp
                         = reinterpret_cast<Segment::CoordinateProjPtr>(f_ptr);
                     return s.partition(cp, n);
                 },
                 py::call_guard<py::gil_scoped_release>())
            .def("angled_partition",
                 [](const Segment &s, float degrees, int n) {
                     return s.partition(degrees, n);
                 },
                 py::call_guard<py::gil_scoped_release>())
            .def("translated", [](const Segment &s, int dx, int dy) {
                return s.translate(dx, dy);
            })
            .def("size", &Segment::size)
            .def("__getitem__", [](const Segment& s, int i) {
                auto const pt = s[i];
                return std::pair<int, int>(pt.x(), pt.y());
            });
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
            segPx.px(i)[cn] = data[i * pxStride + cn * cnStride];

    return segPx;
}

py::buffer_info segmentPixelsBuffer(const SegmentPixels &segPx) {
    auto spu = segPx.unrestricted();
    return {
            spu.px(0),
            sizeof(float),
            py::format_descriptor<float>::format(),
            2,
            {spu.size(), spu.depth()},
            {sizeof(float) * spu.depth(),
             sizeof(float)}
    };
}

void bindSegmentPixels(py::module_ &m) {
    py::class_<SegmentPixels>(m, "SegmentPixels", py::buffer_protocol())
            .def(py::init(&pyBufferToSegmentPixels))
            .def("asdf_restriction", &SegmentPixels::asdfRestriction,
                 py::call_guard<py::gil_scoped_release>())
            .def("filter_restriction", &SegmentPixels::filterRestriction,
                 py::call_guard<py::gil_scoped_release>())
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
            .def_static("create_pseudo_bubble_sorter", &Sorter::pseudoBubble)
            .def("__call__", &Sorter::operator(),
                 py::call_guard<py::gil_scoped_release>());
}

void bindEllipse(py::module_ &m) {
    py::class_<Ellipse>(m, "Ellipse")
            .def(py::init<float, float, float>())
            .def(py::init<float, float, float, float, float>())
            .def("contains_point", [](const Ellipse &e, float x, float y) {
                return e.containsPoint(Point2f(x, y));
            })
            .def("translated", &Ellipse::translate)
            .def("rotated", &Ellipse::rotate)
            .def("scaled", &Ellipse::scale)
            .def("modulated", &Ellipse::modulate);
}

void bindPolygon(py::module_ &m) {
    py::class_<Polygon>(m, "Polygon")
            .def(py::init<const std::vector<std::pair<float, float>> &>())
            .def(py::init<float, float, float, float>())
            .def("contains_point", [](const Polygon &p, float x, float y) {
                return p.containsPoint(Point2f(x, y));
            })
            .def("sheared", &Polygon::shear)
            .def("translated", &Polygon::translate)
            .def("rotated", &Polygon::rotate)
            .def("scaled", &Polygon::scale)
            .def("modulated", &Polygon::modulate,
                 py::call_guard<py::gil_scoped_release>());
}


void bindSkew(py::module_ &m) {
    py::enum_<Skew::OutOfBoundsPolicy>(m, "OutOfBoundsPolicy",
       "The policy to apply to determine the skew of points lying outside of "
       "a Skew's configured bounds.")
            .value("Wrap", Skew::WRAP)
            .value("Clamp", Skew::CLAMP);

    py::class_<Skew>(m, "Skew")
            .def(py::init())
            .def(py::init([](const std::vector<std::pair<int, int>>& s,
                             Skew::OutOfBoundsPolicy oobp){
                std::vector<Point> pts(s.size());
                for (int i = 0; i < s.size(); i++) {
                    auto &[x, y] = s[i];
                    pts[i] = Point(x, y);
                }
                return Skew(pts, oobp);
            }))
            .def(py::init([](uint64_t fPtr, int im_w, int im_h, int im_d,
                             Skew::OutOfBoundsPolicy p ){
                Skew::SkewFunction f = reinterpret_cast<Skew::SkewFnPtr>(fPtr);
                return Skew(f, im_w,  im_h, im_d, p);
            }), py::call_guard<py::gil_scoped_release>())
            .def("scaled", &Skew::scale)
            .def("rotated", &Skew::rotate)
            .def("translated", &Skew::translate)
            .def("__call__", &Skew::operator());
}

void bindModulator(py::module_ &m) {
    py::class_<modulator::Modulator>(m, "Modulator")
            .def(py::init([](uint64_t fp) -> modulator::Modulator {
                return {reinterpret_cast<float(*)(float)>(fp)};
            }))
            .def("__add__", &modulator::sum)
            .def("__add__", &modulator::phaseShift)
            .def("__mul__", &modulator::freqShift)
            .def("__call__", &modulator::Modulator::operator())
            .def_static("identity", &modulator::identity)
            .def_static("sine_wave", &modulator::sinOscillator)
            .def_static("triangle_wave", &modulator::triangleOscillator)
            .def_static("saw_wave", &modulator::sawOscillator)
            .def_static("square_wave", &modulator::squareOscillator)
            .def_static("stepwise", &modulator::stepwise);
}