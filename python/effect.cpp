#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Effect.h"
#include "effect/BucketSort.h"
#include "effect/PartialBubbleSort.h"
#include "effect/PartialHeapify.h"
#include "Segment.h"

using namespace pxsort;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(PixelComparator);
PYBIND11_MAKE_OPAQUE(PixelMixer);
PYBIND11_MAKE_OPAQUE(PixelPredicate);

void init_effect(py::module_ &m) {

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
            .def(py::init<const ChannelSkew&, SegmentTraversal,
                          PixelProjection, PixelMixer, int>());
}

//class PyEffect : public Effect {
//public:
//    using Effect::Effect;
//
//    void attachToSegment(Segment &segment) override {
//        PYBIND11_OVERRIDE_PURE(void, Effect, attachToSegment, segment);
//    }
//
//    [[nodiscard]] Effect * clone_wrapper() const {
//        PYBIND11_OVERLOAD_PURE(Effect *, Effect, clone, );
//    }
//
//    void applyToSegment(Segment &segment) override {
//        PYBIND11_OVERRIDE_PURE(void, Effect, applyToSegment, segment);
//    }
//};

//    py::class_<Effect, PyEffect>
//            (m, "Effect",
//             "An Effect is associated with a specific Segment. "
//             "Each time that apply() is called, an Effect will applyToSegment its "
//             "effect to the given Segment, mutating the underlying Image in the "
//             "process.\n"
//             "Although both attachToSegment and applyToSegment take a Segment as a parameter, "
//             "the same Segment that was attached to an effect must be given to "
//             "applyToSegment. Not doing so may result in unspecified behaviour.\n"
//             "In practice, applyToSegment should never be called explicitly by the "
//             "library user. Instead, it is called by a Segment through a "
//             "double-dispatch mechanism.")
//             .def(py::init<const ChannelSkew&, SegmentTraversal>())
//             .def("attach_to_segment", &Effect::attachToSegment)
//             .def("apply_to_segment", &Effect::applyToSegment)
//             .def("__copy__", [](const Effect &self) {
//                 return self.clone();
//             })
//             .def("__deepcopy__", [](const Effect &self, py::dict){
//                 return self.clone();
//             });