#include <boost/python.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "JNet/JControlHostObjectIterator.hh"

#include "onlineeventmanager/KM3OnlineEvent.hh"
#include "onlineeventmanager/KM3OnlineTags.hh"

namespace bp = boost::python;

struct JControlHostObjectIteratorPy:
  JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent>
{
  JControlHostObjectIteratorPy(const std::string hostName):
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent>(hostName)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineEvent>( *this->next() );
  } 

  virtual ~JControlHostObjectIteratorPy()
  {}

};


void export_JControlHostObjectIteratorPy()
{
  bp::class_<JControlHostObjectIteratorPy,boost::noncopyable>("JControlHostObjectIterator",
							      bp::init<const std::string>())
     
     .def("hasNext", &JControlHostObjectIteratorPy::hasNext)
     .def("next"   , &JControlHostObjectIteratorPy::next_event, bp::return_value_policy<bp::return_by_value>())
     ;
}


