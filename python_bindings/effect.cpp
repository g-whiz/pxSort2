#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Sorter.h"
#include "effect/BucketSort.h"
#include "effect/PartialBubbleSort.h"
#include "effect/PartialHeapify.h"
#include "Segment.h"
#include "wrapper.h"

using namespace pxsort;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelComparator);
PYBIND11_MAKE_OPAQUE(PixelMixer);

std::shared_ptr<BucketSort> makeBucketSort(const ChannelSkew& skew,
                                           SegmentTraversal traversal,
                                           const PixelProjectionWrapper& pw,
                                           const PixelMixer& mix,
                                           int nBuckets) {
    return std::make_shared<BucketSort>(skew, traversal, pw.projection, mix, nBuckets);
}

void init_effect(py::module_ &base) {
    auto m = base.def_submodule("sorter");

    py::class_<Sorter, std::shared_ptr<Sorter>>
            (m, "Sorter")
            .def_readwrite("skew", &Sorter::skew)
            .def_readwrite("traversal", &Sorter::traversal);

    py::class_<PartialBubbleSort, Sorter, std::shared_ptr<PartialBubbleSort>>
            (m, "PartialBubbleSort",
             "This Sorter applies one \"pass\" of the bubble sort algorithm "
             "each time applyToSegment is invoked.")
             .def(py::init<const ChannelSkew&, SegmentTraversal,
                           PixelComparator, PixelMixer>());

    py::class_<PartialHeapify, Sorter, std::shared_ptr<PartialHeapify>>
            (m, "PartialHeapify",
             "This Sorter applies one bubble-down \"pass\" of the heapify "
             "algorithm each time applyToSegment is invoked.")
             .def(py::init<const ChannelSkew&, SegmentTraversal,
                           PixelComparator, PixelMixer>());

    py::class_<BucketSort, Sorter, std::shared_ptr<BucketSort>>
            (m, "BucketSort",
             "This Sorter applies the bucket-sort algorithm to sort the pixels "
             "in a segment when applyToSegment is invoked.")
            .def(py::init(&makeBucketSort));
}