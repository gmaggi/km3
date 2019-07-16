#include <boost/python.hpp>

void export_JDAQHeader();
void export_KM3OnlineTrack();  
void export_KM3OnlineEvent();  
void export_KM3OnlineMultiVariables();
void export_KM3OnlineScoredEvent();

BOOST_PYTHON_MODULE(km3online)
{
  export_JDAQHeader();
  export_KM3OnlineTrack();
  export_KM3OnlineEvent();
  export_KM3OnlineMultiVariables();
  export_KM3OnlineScoredEvent();
}
