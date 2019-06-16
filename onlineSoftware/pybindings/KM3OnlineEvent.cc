#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/operators.hpp>

#include "onlineeventmanager/KM3OnlineEvent.hh"

namespace bp = boost::python;

void export_KM3OnlineEvent()
{
  using KM3ONLINE::KM3OnlineEvent;
  
  bp::class_<KM3OnlineEvent, boost::shared_ptr<KM3OnlineEvent> >( "KM3OnlineEvent",
								  bp::init<>() ) 
			   
    .def("getRecoShower",          &KM3OnlineEvent::GetRecoShower)
    .def("getRecoTrack",           &KM3OnlineEvent::GetRecoTrack)
    .def("getDetectorID",          &KM3OnlineEvent::GetDetectorID)
    .def("getRunNumber",           &KM3OnlineEvent::GetRunNumber)
    .def("getFrameIndex",          &KM3OnlineEvent::GetFrameIndex)
    .def("getTriggerCounter",      &KM3OnlineEvent::GetTriggerCounter)
    .def("getUTCTimeSliceStartSec",&KM3OnlineEvent::GetUTCTimeSliceStartSec)
    .def("getMultiVariables",      &KM3OnlineEvent::GetMultiVariables)
    .def(bp::self_ns::str(bp::self_ns::self))
    ;
}
