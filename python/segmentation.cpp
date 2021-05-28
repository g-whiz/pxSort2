#include <pybind11/pybind11.h>

#include "Image.h"
#include "Effect.h"
#include "Segment.h"
#include "Segmentation.h"
#include "segmentation/Grid.h"

using namespace pxsort;
namespace py = pybind11;

/* Wrapper to release/reacquire GIL before/after calling applyEffects,
 * which uses OpenMP to achieve parallelism. */
void pyApplyEffects(const std::shared_ptr<Segmentation>& s) {
    py::gil_scoped_release release;

    s->applyEffects();

    py::gil_scoped_acquire acquire;
}


void init_segmentation(py::module &m) {

    py::class_<Segmentation, std::shared_ptr<Segmentation>>
            (m, "Segmentation")
            .def("__len__", &Segmentation::size,
                 "Return the number of segments in this segmentation.")
            .def("get_segment", &Segmentation::getSegment)
            .def("add_effect", py::overload_cast<std::shared_ptr<Effect>>(
                                   &Segmentation::addEffect),
                 "Add an effect to all segments in this Segmentation.")
            .def("add_effect", py::overload_cast<std::shared_ptr<Effect>, int>(
                    &Segmentation::addEffect),
                 "Add an effect to the ith segment in this Segmentation.")
            .def("apply_effects", &pyApplyEffects,
                 "Apply the effects attached to each Segment in this "
                 "Segmentation once.");

    py::class_<Grid, Segmentation, std::shared_ptr<Grid>>
            (m, "Grid")
            .def(py::init<const std::shared_ptr<Image>&, int, int, int, int>());

}



