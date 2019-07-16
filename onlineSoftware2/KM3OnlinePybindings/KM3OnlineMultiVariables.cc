#include <boost/python.hpp>
#include <boost/python/operators.hpp>

#include "KM3Online/KM3OnlineMultiVariables.hh"

namespace bp = boost::python;

void export_KM3OnlineMultiVariables()
{
  using KM3ONLINE::KM3OnlineMultiVariables;

  bp::class_<KM3OnlineMultiVariables>("KM3OnlineMultiVariables", 
				      bp::init<>())
     
    .def("getCoC",           &KM3OnlineMultiVariables::getCoC)
    .def("getToT",           &KM3OnlineMultiVariables::getToT)
    .def("getChargeAbove",   &KM3OnlineMultiVariables::getChargeAbove)
    .def("getChargeBelow",   &KM3OnlineMultiVariables::getChargeBelow)
    .def("getChargeRatio",   &KM3OnlineMultiVariables::getChargeRatio)
    .def("getDeltaPosZ",     &KM3OnlineMultiVariables::getDeltaPosZ)
    .def("getNTriggeredHits",&KM3OnlineMultiVariables::getNTriggeredHits)
    .def("getNSnapPosZ",     &KM3OnlineMultiVariables::getNSnapHits)
    .def(bp::self_ns::str(bp::self_ns::self))
  ;
}
