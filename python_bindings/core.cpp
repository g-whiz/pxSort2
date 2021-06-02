#include <pybind11/pybind11.h>

#include "common.h"

using namespace pxsort;
namespace py = pybind11;


int getSkewVal(const std::shared_ptr<ChannelSkew> &skew, const py::tuple& key) {
    int channel = key[0].cast<int>();
    int dim = key[1].cast<int>();

    return (*skew)(PXSORT_MODULO(channel, 3), PXSORT_MODULO(dim, 2));
}

void setSkewVal(const std::shared_ptr<ChannelSkew> &skew,
               const py::tuple& key, int val) {
    int channel = key[0].cast<int>();
    int dim = key[1].cast<int>();

    (*skew)(PXSORT_MODULO(channel, 3), PXSORT_MODULO(dim, 2)) = val;
}

void init_core(py::module_ &m) {

    py::enum_<Channel>
            (m, "Channel")
            .value("R", R)
            .value("G", G)
            .value("B", B);

    py::class_<ChannelSkew, std::shared_ptr<ChannelSkew>>
        (m, "ChannelSkew")
        .def(py::init<int, int, int, int, int, int>())
        .def("__getitem__", &getSkewVal)
        .def("__setitem__", &setSkewVal);

}