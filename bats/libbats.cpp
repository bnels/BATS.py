#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <bats.h>
#include <vector>
#include <iostream>

namespace py = pybind11;

using IntVec = SparseVector<int, size_t>;
using IntMat = ColumnMatrix<IntVec>;
using F2 = ModP<int, 2>;
using F3 = ModP<int, 3>;
using F5 = ModP<int, 5>;

using VInt = SparseVector<int, size_t>;
using V2 = SparseVector<F2, size_t>;
using V3 = SparseVector<F3, size_t>;
using V5 = SparseVector<F5, size_t>;

using M2 = ColumnMatrix<V2>;
using M3 = ColumnMatrix<V3>;

// interface of common functions between F2 and other modP
#define BasicModPInterface(F, name) py::class_<F>(m, name)\
.def(py::init<>())\
.def(py::init<int>())\
.def("__add__", &F::operator+)\
.def("__mul__", &F::operator*)\
.def("__sub__", py::overload_cast<const F&>(&F::operator-, py::const_))\
.def("__truediv__", &F::operator/)\
.def("__str__", &F::str)

// interface for everything other than F2
#define ModPInterface(F, name) BasicModPInterface(F, name)\
.def("__neg__", py::overload_cast<>(&F::operator-, py::const_));

#define SparseVectorInterface(F, name) py::class_<SparseVector<F, size_t>>(m, name)\
.def(py::init<>())\
.def(py::init<const std::vector<size_t>&, const std::vector<F>&>())\
.def("__getitem__", py::overload_cast<size_t>(&SparseVector<F, size_t>::getval, py::const_))\
.def("print", &SparseVector<F, size_t>::print);
// .def("__getitem__", (F (SparseVector<F, size_t>::*)(size_t))(&SparseVector<F, size_t>::operator[]))

#define ColumnMatrixInterface(VT, name) py::class_<ColumnMatrix<VT>>(m, name)\
.def(py::init<>())\
.def(py::init<size_t, size_t>())\
.def("print", &ColumnMatrix<VT>::print);

#define ChainComplexInterface(MT, name) py::class_<ChainComplex<MT>>(m, name)\
.def(py::init<>())\
.def(py::init<const SimplicialComplex&>())\
.def("__getitem__", &ChainComplex<MT>::operator[]);

#define ReducedChainComplexInterface(MT, name) py::class_<ReducedChainComplex<MT>>(m, name)\
.def(py::init<>())\
.def(py::init<const ChainComplex<MT>&>())\
.def("hdim", &ReducedChainComplex<MT>::hdim)\
.def("get_preferred_representative", &ReducedChainComplex<MT>::get_preferred_representative)\
.def("find_preferred_representative", &ReducedChainComplex<MT>::find_preferred_representative)\
.def("maxdim", &ReducedChainComplex<MT>::maxdim);

PYBIND11_MODULE(libbats, m) {
    m.doc() = "Basic Applied Topology Subprograms interface";

    BasicModPInterface(F2, "F2")
        .def("__neg__", py::overload_cast<>(&F2::operator-));

    ModPInterface(F3, "F3")
    ModPInterface(F5, "F5")

    SparseVectorInterface(int, "IntVector")
    SparseVectorInterface(F2, "F2Vector")
    SparseVectorInterface(F3, "F3Vector")

    ColumnMatrixInterface(VInt, "IntMat")
    ColumnMatrixInterface(V2, "F2Mat")
    ColumnMatrixInterface(V3, "F3Mat")


    py::class_<CSCMatrix<int, size_t>>(m, "CSCMatrix")
        .def(py::init<>())
        .def(py::init<size_t, size_t, const std::vector<size_t> &, const std::vector<size_t> &, const std::vector<int> &>())
        .def("__call__", &CSCMatrix<int, size_t>::getval)
        .def("print", py::overload_cast<>(&CSCMatrix<int, size_t>::print, py::const_));

    py::class_<cell_ind>(m, "cell_ind")
        .def(py::init<>())
        .def(py::init<size_t, size_t>());

    py::class_<SimplicialComplex>(m, "SimplicialComplex")
        .def(py::init<>())
        .def("maxdim", &SimplicialComplex::maxdim, "maximum dimension simplex")
        .def("ncells", py::overload_cast<>(&SimplicialComplex::ncells, py::const_), "number of cells")
        .def("ncells", py::overload_cast<const size_t>(&SimplicialComplex::ncells, py::const_), "number of cells in given dimension")
        .def("add", (cell_ind (SimplicialComplex::*)(std::vector<size_t>&))( &SimplicialComplex::add ), "add simplex")
        .def("find_idx", py::overload_cast<const std::vector<size_t> &>(&SimplicialComplex::find_idx))
        .def("boundary", &SimplicialComplex::boundary_csc)
        .def("print_summary", &SimplicialComplex::print_summary);

    py::class_<CellularMap>(m, "CellularMap")
        .def(py::init<>())
        .def(py::init<size_t>())
        .def("__getitem__", py::overload_cast<size_t>(&CellularMap::operator[], py::const_))\
        .def("__setitem__", py::overload_cast<size_t>(&CellularMap::operator[]));

    ChainComplexInterface(M2, "F2ChainComplex")
    ChainComplexInterface(M3, "F3ChainComplex")

    ReducedChainComplexInterface(M2, "ReducedF2ChainComplex")
    ReducedChainComplexInterface(M3, "ReducedF3ChainComplex")
}
