#include <boost/python.hpp>
#include <boost/python/operators.hpp>

#include "KM3Online/KM3OnlineTrack.hh"

namespace bp = boost::python;

void export_KM3OnlineTrack()
{
  using KM3ONLINE::KM3OnlineTrack;

  bp::class_<KM3OnlineTrack>("KM3OnlineTrack", 
			     bp::init<const double,const double,const double,
			              const double,const double,const double,
			              const double,const int   ,const int,
			              const double,const double,const int>() )
     
    .def("getX",      &KM3OnlineTrack::getX)
    .def("getY",      &KM3OnlineTrack::getY)
    .def("getZ",      &KM3OnlineTrack::getZ)
    .def("getDX",     &KM3OnlineTrack::getDX)
    .def("getDY",     &KM3OnlineTrack::getDY)
    .def("getDZ",     &KM3OnlineTrack::getDZ)
    .def("getEnergy", &KM3OnlineTrack::getEnergy)
    .def("getQuality",&KM3OnlineTrack::getQuality)
    .def("getTime",   &KM3OnlineTrack::getTime)
    .def("getType",   &KM3OnlineTrack::getType)
    .def("getStatus", &KM3OnlineTrack::getStatus)
    .def("getNDF",    &KM3OnlineTrack::getNDF)
    .def(bp::self_ns::str(bp::self_ns::self))
    ;
}
