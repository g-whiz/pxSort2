#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Effect.h"
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
    auto m = base.def_submodule("effect");

    py::class_<Effect, std::shared_ptr<Effect>>
            (m, "Effect")
            .def_readwrite("skew", &Effect::skew)
            .def_readwrite("traversal", &Effect::traversal);

    py::class_<PartialBubbleSort, Effect, std::shared_ptr<PartialBubbleSort>>
            (m, "PartialBubbleSort",
             "This Effect applies one \"pass\" of the bubble sort algorithm "
             "each time applyToSegment is invoked.")
             .def(py::init<const ChannelSkew&, SegmentTraversal,
                           PixelComparator, PixelMixer>());

    py::class_<PartialHeapify, Effect, std::shared_ptr<PartialHeapify>>
            (m, "PartialHeapify",
             "This Effect applies one bubble-down \"pass\" of the heapify "
             "algorithm each time applyToSegment is invoked.")
             .def(py::init<const ChannelSkew&, SegmentTraversal,
                           PixelComparator, PixelMixer>());

    py::class_<BucketSort, Effect, std::shared_ptr<BucketSort>>
            (m, "BucketSort",
             "This Effect applies the bucket-sort algorithm to sort the pixels "
             "in a segment when applyToSegment is invoked.")
            .def(py::init(&makeBucketSort));
}