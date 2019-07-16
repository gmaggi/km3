#include <boost/python.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "JNet/JLigierObjectIterator.hh"

#include "KM3Online/KM3OnlineScoredEvent.hh"
#include "KM3Online/KM3OnlineEvent.hh"
#include "KM3Online/KM3OnlineTags.hh"

namespace bp = boost::python;

struct JLigierObjItKM3OnlineEvent:
  JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineEvent>
{
  JLigierObjItKM3OnlineEvent(const int port, const int backlog):
    JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineEvent>(port,backlog)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineEvent>( *this->next() );
  } 

  virtual ~JLigierObjItKM3OnlineEvent()
  {}

};


struct JLigierObjItKM3OnlineScoredEvent:
  JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineScoredEvent>
{
  JLigierObjItKM3OnlineScoredEvent(const int port, const int backlog):
    JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineScoredEvent>(port,backlog)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineScoredEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineScoredEvent>( *this->next() );
  } 

  virtual ~JLigierObjItKM3OnlineScoredEvent()
  {}

};


void export_JLigierObjItKM3OnlineEvent()
{
  bp::class_<JLigierObjItKM3OnlineEvent>("JLigierObjIt_KM3OnlineEvent",
					 bp::init<const int,const int>())
    
    .def("hasNext", &JLigierObjItKM3OnlineEvent::hasNext)
    .def("next"   , &JLigierObjItKM3OnlineEvent::next_event, 
	 bp::return_value_policy<bp::return_by_value>())
    ;
}


void export_JLigierObjItKM3OnlineScoredEvent()
{
  bp::class_<JLigierObjItKM3OnlineScoredEvent>("JLigierObjIt_KM3OnlineScoredEvent",
					       bp::init<const int,const int>())
    
    .def("hasNext", &JLigierObjItKM3OnlineScoredEvent::hasNext)
    .def("next"   , &JLigierObjItKM3OnlineScoredEvent::next_event, 
	 bp::return_value_policy<bp::return_by_value>())
    ;
}


