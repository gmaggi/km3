#include <boost/python.hpp>

#include "JDAQ/JDAQHeader.hh"

namespace bp = boost::python;

void export_JDAQHeader()
{
  using KM3NETDAQ::JDAQHeader;
  
  bp::class_<JDAQHeader>( "JDAQHeader",
			  bp::init<>() ) 
			   
    .def("getDetectorID", &JDAQHeader::getDetectorID)
    .def("getRunNumber" , &JDAQHeader::getRunNumber)
    .def("getFrameIndex", &JDAQHeader::getFrameIndex)
    ;
}
