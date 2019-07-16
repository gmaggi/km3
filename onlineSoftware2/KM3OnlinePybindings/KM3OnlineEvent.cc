#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/operators.hpp>

#include "KM3Online/KM3OnlineEvent.hh"

namespace bp = boost::python;

void export_KM3OnlineEvent()
{
  using KM3ONLINE::KM3OnlineEvent;
  
  bp::class_<KM3OnlineEvent, bp::bases<KM3NETDAQ::JDAQHeader>, 
	     boost::shared_ptr<KM3OnlineEvent> >( "KM3OnlineEvent",
						  bp::init<>() ) 
			   
    .def("getTrack",           &KM3OnlineEvent::getTrack)
    .def("getMultiVariables",  &KM3OnlineEvent::getMultiVariables)
    .def(bp::self_ns::str(bp::self_ns::self))
    ;
}
