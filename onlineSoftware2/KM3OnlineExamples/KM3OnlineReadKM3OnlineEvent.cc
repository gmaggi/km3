#include <iostream>
#include <string>

#include "JDAQ/JDAQKeyHit.hh"
#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"

#include <KM3Online/KM3OnlineEvent.hh>
#include <KM3Online/KM3OnlineTrack.hh>
#include <KM3Online/KM3OnlineTags.hh>



int main(int argc, char **argv)
{
  using KM3ONLINE::KM3OnlineEvent;
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
    JNET::JControlHostObjectIterator<KM3OnlineEvent> data(HOSTNAME);

    while ( data.hasNext() ){

      const KM3OnlineEvent *event = data.next();

      cout<<*event<< endl;

      for (std::vector<JDAQKeyHit>::const_iterator hit = event->begin<JDAQKeyHit>(); 
	   hit != event->end<KM3NETDAQ::JDAQKeyHit>(); ++hit){
	DEBUG("time snap hits "<<hit->getT()<<endl);
      }

      for (std::vector<JDAQTriggeredHit>::const_iterator hit = event->begin<JDAQTriggeredHit>(); 
	   hit != event->end<JDAQTriggeredHit>(); ++hit){
	DEBUG("time trig hits "<<hit->getT()<<endl);
      }

      cout<<"size BEFORE removing (by <>): "<<event->size<JDAQTriggeredHit>()<<endl;

      std::vector<JDAQTriggeredHit> noConstTrigHits= event->getHits<JDAQTriggeredHit>();
      cout<<"size BEFORE removing: "<<noConstTrigHits.size()<<endl;

      if ( not noConstTrigHits.empty() ){
	noConstTrigHits.erase(noConstTrigHits.begin(),noConstTrigHits.begin()+1);
      }
      cout<<"size AFTER removing: "<<noConstTrigHits.size()<<endl;

      const KM3ONLINE::KM3OnlineTrack &track = event->getTrack();
      cout<<"TRACK DZ: "<<track.getDZ()<<endl;
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineReadKM3OnlineEvent " << endl);
  }
  
  return 0;
}

  
