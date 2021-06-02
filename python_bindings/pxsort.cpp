#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_core(py::module_ &m);
void init_image(py::module_ &m);
void init_segment(py::module_ &m);
void init_effect(py::module_ &m);
void init_segmentation(py::module_ &m);
void init_mixer(py::module_ &m);
void init_comparator(py::module_ &m);
void init_projection(py::module_ &m);
void init_predicate(py::module_ &m);

PYBIND11_MODULE(pxsort, m) {
    init_core(m);
    init_image(m);
    init_effect(m);
    init_segment(m);
    init_segmentation(m);
    init_mixer(m);
    init_comparator(m);
    init_projection(m);
    init_predicate(m);
}