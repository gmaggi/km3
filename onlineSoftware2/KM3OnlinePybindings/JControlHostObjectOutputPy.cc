#include <boost/python.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "KM3Online/KM3OnlineTags.hh"

#include "JNet/JControlHostObjectOutput.hh"
#include "JNet/JHostname.hh"

#include "KM3Online/KM3OnlineScoredEvent.hh"
#include "KM3Online/KM3OnlineDataTypes.hh"


struct JControlHostObjOutKM3OnlineScoredEvent:
  JNET::JControlHostObjectOutput<KM3ONLINE::KM3OnlineScoredEvent>
{
  JControlHostObjOutKM3OnlineScoredEvent(const std::string &buffer):
    JNET::JControlHostObjectOutput<KM3ONLINE::KM3OnlineScoredEvent>(JNET::JHostname(buffer))//JNET::JTag("IO_SCORE"))
  {}

  virtual ~JControlHostObjOutKM3OnlineScoredEvent()
  {}

};


void export_JControlHostObjOutKM3OnlineScoredEvent()
{
  namespace bp = boost::python;
  
  bp::class_<JControlHostObjOutKM3OnlineScoredEvent,boost::noncopyable>("JControlHostObjOut_KM3OnlineScoredEvent",
									bp::init<const std::string&>())
     
    .def("put", &JControlHostObjOutKM3OnlineScoredEvent::put)
    ;
}

