#include <boost/python.hpp>
#include <boost/python/return_by_value.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "JNet/JLigierObjectIterator.hh"

#include "onlineeventmanager/KM3OnlineEvent.hh"
#include "onlineeventmanager/KM3OnlineTags.hh"

namespace bp = boost::python;

struct JLigierObjectIteratorPy:
  JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineEvent>
{
  JLigierObjectIteratorPy(const int port, const int backlog):
    JNET::JLigierObjectIterator<KM3ONLINE::KM3OnlineEvent>(port,backlog)
  {}

  boost::shared_ptr<KM3ONLINE::KM3OnlineEvent> next_event()
  {
    return boost::make_shared<KM3ONLINE::KM3OnlineEvent>( *this->next() );
  } 

  virtual ~JLigierObjectIteratorPy()
  {}

};


void export_JLigierObjectIteratorPy()
{
  bp::class_<JLigierObjectIteratorPy>("JLigierObjectIterator",
				      bp::init<const int,const int>())
    
    .def("hasNext", &JLigierObjectIteratorPy::hasNext)
    .def("next"   , &JLigierObjectIteratorPy::next_event, bp::return_value_policy<bp::return_by_value>())
    ;
}


