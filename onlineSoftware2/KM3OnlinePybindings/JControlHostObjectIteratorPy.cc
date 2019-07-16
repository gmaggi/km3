#include <boost/python.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "JNet/JControlHostObjectIterator.hh"

#include "KM3Online/KM3OnlineScoredEvent.hh"
#include "KM3Online/KM3OnlineEvent.hh"
#include "KM3Online/KM3OnlineTags.hh"

namespace bp = boost::python;


struct JControlHostObjItKM3OnlineEvent:
  JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent>
{
  JControlHostObjItKM3OnlineEvent(const std::string hostName):
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent>(hostName)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineEvent>( *this->next() );
  } 

  virtual ~JControlHostObjItKM3OnlineEvent()
  {}

};


struct JControlHostObjItKM3OnlineScoredEvent:
  JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineScoredEvent>
{
  JControlHostObjItKM3OnlineScoredEvent(const std::string hostName):
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineScoredEvent>(hostName)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineScoredEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineScoredEvent>( *this->next() );
  } 

  virtual ~JControlHostObjItKM3OnlineScoredEvent()
  {}

};



void export_JControlHostObjItKM3OnlineEvent()
{
  bp::class_<JControlHostObjItKM3OnlineEvent,boost::noncopyable>("JControlHostObjIt_KM3OnlineEvent",
								 bp::init<const std::string>())
     
    .def("hasNext", &JControlHostObjItKM3OnlineEvent::hasNext)
    .def("next"   , &JControlHostObjItKM3OnlineEvent::next_event, 
	 bp::return_value_policy<bp::return_by_value>())
    ;
}


void export_JControlHostObjItKM3OnlineScoredEvent()
{
  bp::class_<JControlHostObjItKM3OnlineScoredEvent,boost::noncopyable>("JControlHostObjIt_KM3OnlineScoredEvent",
								       bp::init<const std::string>())
     
     .def("hasNext", &JControlHostObjItKM3OnlineScoredEvent::hasNext)
     .def("next"   , &JControlHostObjItKM3OnlineScoredEvent::next_event, 
	  bp::return_value_policy<bp::return_by_value>())
     ;
}


