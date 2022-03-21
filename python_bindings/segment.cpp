#include <pybind11/pybind11.h>

#include "Image.h"
#include "Segment.h"
#include "segment/Asendorf.h"
#include "segment/Rectangle.h"
#include "wrapper.h"

namespace py = pybind11;
using namespace pxsort;

std::shared_ptr<Asendorf> makeAsendorf(const std::shared_ptr<Segment>& segment,
                                       PixelPredicateWrapper &lo,
                                       PixelPredicateWrapper &hi) {
    return std::make_shared<Asendorf>(segment, lo.predicate, hi.predicate);
}

void init_segment(py::module_ &base) {
    auto m = base.def_submodule("segment");

    py::enum_<Segment::Traversal>(m, "Traversal",
                                  "Traversal options for a Segment's pixels.")
            .value("Forward", FORWARD)
            .value("Reverse", REVERSE)
            .value("BinaryTreeBreadthFirst", BINARY_TREE_BREADTH_FIRST);

    py::class_<Segment, std::shared_ptr<Segment>>
            (m, "Segment")
            .def("__len__", &Segment::size,
                 "Returns the number of Pixels in this Segment.")
            .def("get_pixel", &Segment::getPixel)
            .def("set_pixel", &Segment::setPixel)
            .def("add_effect", &Segment::addEffect)
            .def("get_effect", &Segment::getEffect)
            .def("num_effects", &Segment::numEffects)
            .def("apply_effects", &Segment::applyEffects);

    py::class_<Rectangle, Segment, std::shared_ptr<Rectangle>>
            (m, "Rectangle", "A rectangular segment of an Image.")
            .def(py::init<std::shared_ptr<Image>, int, int, int, int>());

    py::class_<Asendorf, Segment, std::shared_ptr<Asendorf>>
            (m, "Asendorf",
            "An Asendorf is a Segment that wraps another Segment so as to "
            "implement an image segmentation similar to that used in Kim "
            "Asendorf's pioneering implementation of a pixel-sorting "
            "algorithm. "
            "In particular, an Asendorf restricts the pixels in a given "
            "Segment to a subset defined by two predicates - loTest and hiTest "
            "- in the following manner: \n"
            "   - let S be the base segment, and let S[i] be the i'th pixel in "
            "S, where 0 <= i < S.size()\n"
            "   - let A be the Asendorf of S, using predicates loTest and "
            "hiTest\n"
            "   - let i_lo = min { 0 <= i < S.size() | loTest(S[i]) == true}\n"
            "   - let i_hi = max { 0 <= i < S.size() | hiTest(S[i]) == true}\n"
            "   - then A.size() = max { 0, i_hi - i_lo + 1}\n"
            "   - if A.size() > 0, then A[i] = S[i + i_lo], for 0 <= i < "
            "A.size()")
            .def(py::init(&makeAsendorf));
}
