#include <boost/python/module.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/class.hpp>
#include <boost/python/pure_virtual.hpp>

#include "Segment.h"
#include "segment/Asendorf.h"
#include "segment/Rectangle.h"

using namespace boost::python;
using namespace pxsort;


class SegmentWrap : public Segment, public wrapper<Segment> {
public:
    int size() override {
        return this->get_override("size")();
    }
};


BOOST_PYTHON_MODULE(segment) {
    enum_<SegmentTraversal>("SegmentTraversal",
                            "Traversal options for a Segment's pixels.")
            .value("Forward", FORWARD)
            .value("Reverse", REVERSE)
            .value("BinaryTreeBreadthFirst", BINARY_TREE_BREADTH_FIRST)
            .export_values();

    class_<SegmentWrap, boost::noncopyable>("Segment",
                                        "Interface for accessing a segment of "
                                        "an image.\n\n"
                                        "The primary purpose of the Segment "
                                        "interface is to provide Effects with "
                                        "a 1-dimensional array-like view of some "
                                        "subset of an image's pixels. In this "
                                        "sense, a Segment maps an arbitrary "
                                        "subset of an Image's pixels to a "
                                        "virtual array.",
                                        no_init)
            .def("size", pure_virtual(&Segment::size));

    // todo: wrap attach to take an auto_ptr
    // todo: asdf + rect segments
}