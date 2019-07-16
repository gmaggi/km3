#include <boost/python.hpp>

void export_JControlHostObjItKM3OnlineEvent();  
void export_JControlHostObjItKM3OnlineScoredEvent();  
void export_JLigierObjItKM3OnlineScoredEvent();
void export_JLigierObjItKM3OnlineEvent();
void export_JControlHostObjOutKM3OnlineScoredEvent();

BOOST_PYTHON_MODULE(jnet)
{
  export_JControlHostObjItKM3OnlineEvent();  
  export_JControlHostObjItKM3OnlineScoredEvent();  
  export_JLigierObjItKM3OnlineScoredEvent();
  export_JLigierObjItKM3OnlineEvent();
  export_JControlHostObjOutKM3OnlineScoredEvent();
}
