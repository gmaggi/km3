#include <boost/python.hpp>

void export_KM3OnlineTrack();  
void export_KM3OnlineEvent();  
void export_KM3OnlineMultiVariables();

BOOST_PYTHON_MODULE(km3online)
{
  export_KM3OnlineTrack();
  export_KM3OnlineEvent();
  export_KM3OnlineMultiVariables();
}
