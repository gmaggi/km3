#include <string>
#include <iostream>
#include <stdexcept>

#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"

#include "onlineeventmanager/KM3OnlineTags.hh"
#include "onlineeventmanager/KM3OnlineEvent.hh"
#include "onlineeventmanager/KM3OnlineTrack.hh"


int main(int argc, char **argv)
{
  JPARSER::JParser<> zap("KM3OnlineFileWriter");
  
  JNET::JHostname HOSTNAME;
  std::string MAINDIR;
  std::string DETECTOR;
  int debug;

  zap['H'] = make_field(HOSTNAME)  ;
   zap['d'] = make_field(debug)     = 1;

  zap.read(argc, argv);

  try{
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent> data(HOSTNAME);

    
    while ( data.hasNext() ){

      const KM3ONLINE::KM3OnlineEvent *in = data.next();
   
      const KM3ONLINE::KM3OnlineTrack &olTrack  = in->GetRecoTrack();
      std::cout<<olTrack<<std::endl;
    }      
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineFileWriter " << std::endl);
  }
      
  return 0;
}

