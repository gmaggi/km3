#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/operators.hpp>

#include "KM3Online/KM3OnlineEvent.hh"
#include "KM3Online/KM3OnlineScoredEvent.hh"

void export_KM3OnlineScoredEvent()
{
  namespace bp = boost::python;
  using KM3ONLINE::KM3OnlineEvent;
  using KM3ONLINE::KM3OnlineScoredEvent;
  
  bp::class_<KM3OnlineScoredEvent, 
	     bp::bases<KM3OnlineEvent>, 
	     boost::shared_ptr<KM3OnlineScoredEvent> >( "KM3OnlineScoredEvent", 
							bp::init<const KM3OnlineEvent&,const double>() ) 
 
    .def("getScore", &KM3OnlineScoredEvent::getScore)
    .def(bp::self_ns::str(bp::self_ns::self))
    ;
}
