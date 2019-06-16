#include <iostream>
#include <string>

#include "JDAQ/JDAQKeyHit.hh"
#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"

#include <onlineeventmanager/KM3OnlineGraphic.hh>
#include <onlineeventmanager/KM3OnlineEvent.hh>
#include <onlineeventmanager/KM3OnlineTrack.hh>
#include <onlineeventmanager/KM3OnlineTags.hh>



int main(int argc, char **argv)
{
  using KM3ONLINE::KM3OnlineGraphic;
  using KM3NETDAQ::JDAQKeyHit;
  using KM3NETDAQ::JDAQTriggeredHit;
  using std::cout;
  using std::endl;

  JPARSER::JParser<> zap("KM3OnlineGraphicRead");

  JNET::JHostname HOSTNAME;
  int debug;

  zap['H'] = make_field(HOSTNAME)     ;
  zap['d'] = make_field(debug)        = 1;

  zap.read(argc, argv);

  try{
    JNET::JControlHostObjectIterator<KM3OnlineGraphic> data(HOSTNAME);

    while ( data.hasNext() ){

      const KM3OnlineGraphic *graphic = data.next();

      cout<<*graphic<< endl;

      for (std::vector<JDAQKeyHit>::const_iterator hit = graphic->begin<JDAQKeyHit>(); 
	   hit != graphic->end<KM3NETDAQ::JDAQKeyHit>(); ++hit){
	cout<<"time snap hits "<<hit->getT()<<endl;
      }

      for (std::vector<JDAQTriggeredHit>::const_iterator hit = graphic->begin<JDAQTriggeredHit>(); 
	   hit != graphic->end<JDAQTriggeredHit>(); ++hit){
	cout<<"time trig hits "<<hit->getT()<<endl;
      }

      cout<<"size BEFORE removing (by <>): "<<graphic->size<JDAQTriggeredHit>()<<endl;

      std::vector<JDAQTriggeredHit> noConstTrigHits= graphic->getHits<JDAQTriggeredHit>();
      cout<<"size BEFORE removing: "<<noConstTrigHits.size()<<endl;

      if ( not noConstTrigHits.empty() ){
	noConstTrigHits.erase(noConstTrigHits.begin(),noConstTrigHits.begin()+1);
      }
      cout<<"size AFTER removing: "<<noConstTrigHits.size()<<endl;

      const KM3ONLINE::KM3OnlineTrack &track = graphic->getTrack();
      cout<<track.getDZ()<<endl;
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineGraphicRead " << endl);
  }
  
  return 0;
}

  
