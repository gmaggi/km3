#include <boost/python.hpp>

void export_JLigierObjectIteratorPy();  
void export_JControlHostObjectIteratorPy();  

BOOST_PYTHON_MODULE(jnet)
{
  export_JLigierObjectIteratorPy();
  export_JControlHostObjectIteratorPy();
}
