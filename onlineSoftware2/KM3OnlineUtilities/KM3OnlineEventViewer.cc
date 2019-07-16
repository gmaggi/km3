#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

#include <KM3Online/KM3OnlineDataTypes.hh>

#include "JDetector/JDetectorToolkit.hh"
#include "JDetector/JModuleRouter.hh"
#include "JDetector/JDetector.hh"
#include "JDAQ/JDAQKeyHit.hh"
#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"

#include <evt/JsonDumper.hh>
#include <evt/Evt.hh>
#include <evt/Hit.hh>
#include <evt/Det.hh>
#include <evt/Trk.hh>

#include <KM3Online/KM3OnlineEvent.hh>
#include <KM3Online/KM3OnlineTrack.hh>
#include <KM3Online/KM3OnlineTags.hh>


inline 
void make_Trks(const KM3ONLINE::KM3OnlineTrack &fit, 
	       std::vector<Trk> &outs)
{
  Trk aaFit;
  
  aaFit.id  = outs.size()+1;
  aaFit.pos = Vec(fit.getX(), fit.getY(), fit.getZ());
  aaFit.dir = Vec(fit.getDX(),fit.getDY(),fit.getDZ());
  aaFit.t   = fit.getTime();
  aaFit.E   = fit.getEnergy();
  aaFit.lik = fit.getQuality();
  //omit other Trk properties, not needed for the viewer

  outs.push_back(aaFit);
}

inline
void make_Hits(const std::vector<KM3NETDAQ::JDAQKeyHit> &jhits,
	       const std::string detectorFile,
	       std::vector<Hit> &aaHits)
{
  JDETECTOR::JDetector detector;
  JDETECTOR::load(detectorFile, detector);

  const JDETECTOR::JModuleRouter router(detector); 

  for (std::vector<KM3NETDAQ::JDAQKeyHit>::const_iterator h = jhits.begin();
       h != jhits.end(); ++h ){

    Hit aaHit;
    aaHit.dom_id     = h->getModuleID();
    aaHit.channel_id = h->getPMT();
    aaHit.tot        = static_cast<unsigned int>(h->getToT());
    aaHit.t          = static_cast<double>(h->getT());
    aaHit.a          = 0;

    const JDETECTOR::JModule &om= router.getModule( h->getModuleID() );

    const int pmt_id = static_cast<int>(h->getPMT());
    const JDETECTOR::JPMT &pmt= om.getPMT(pmt_id);
    const JGEOMETRY3D::JDirection3D &pmtDir=pmt.getDirection();
    const JGEOMETRY3D::JPosition3D &pmtPos=pmt.getPosition();

    aaHit.pos = Vec(pmtPos.getX(), pmtPos.getY(), pmtPos.getZ());
    aaHit.dir = Vec(pmtDir.getDX(), pmtDir.getDY(), pmtDir.getDZ());

    aaHits.push_back(aaHit);

  }
}


int main(int argc, char **argv)
{
  using KM3ONLINE::KM3OnlineEvent;
  using KM3NETDAQ::JDAQKeyHit;
  using KM3ONLINE::KM3OnlineTrack;

  namespace time = std::chrono;

  JPARSER::JParser<> zap("KM3OnlineEventViewer");

  JNET::JHostname HOSTNAME;
  std::string WWWDIR;
  std::string WWWDIR_DZ;
  std::string DETECTORFILE;
  double DMITRYVAR;
  int MINUTES;
  int debug;

  zap['H'] = make_field(HOSTNAME)     ;
  zap['m'] = make_field(MINUTES)      = 10;
  zap['a'] = make_field(WWWDIR)       ;
  zap['s'] = make_field(WWWDIR_DZ)    ;
  zap['v'] = make_field(DMITRYVAR)    = 15.0; 
  zap['D'] = make_field(DETECTORFILE) ;
  zap['d'] = make_field(debug)        = 1;

  zap.read(argc, argv);

  const time::minutes subRunDuration( MINUTES ); 

  time::time_point<time::system_clock> future  
    = time::system_clock::now()+subRunDuration;

  Det detector(DETECTORFILE);
  std::vector<KM3OnlineEvent> onlineEvents;

  try{
    JNET::JControlHostObjectIterator<KM3OnlineEvent> data(HOSTNAME);

    while ( data.hasNext() ){

      const KM3OnlineEvent *onlineEvent = data.next();
      DEBUG(*onlineEvent<< std::endl);

      onlineEvents.push_back(*onlineEvent);

      if ( time::system_clock::now() >= future ){
	
	std::vector<KM3OnlineEvent>::const_iterator selectedEvent=
          std::max_element( onlineEvents.begin(), onlineEvents.end(),
			    [] (const KM3OnlineEvent &lhs, const KM3OnlineEvent &rhs)
			    { return lhs.getTrack().getNDF() < rhs.getTrack().getNDF(); }
			    );
	
	Evt event;
	make_Trks( selectedEvent->getTrack(), event.trks);
	const std::vector<JDAQKeyHit> &hits = selectedEvent->getHits<JDAQKeyHit>();
	make_Hits( hits, DETECTORFILE, event.hits);
	
	detector.apply(event.hits);      

	write_aa3d(event,detector,WWWDIR,"online_event.js.gz","",false);

	const std::time_t &systemTime = time::system_clock::to_time_t( time::system_clock::now() );
	STATUS("Event-display file created at: "<<std::ctime(&systemTime)<<std::endl);

	//Store events

	std::time_t rawtime;
	struct tm * timeinfo;
	char buffer [100];

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);

	std::strftime (buffer,100,"%d.%m.%H.%M.%S",timeinfo);

	const int run = onlineEvent->getRunNumber();
	
	const std::string label = std::to_string(run)+"."+buffer;  
	const std::string name = "Run"+label+".js.gz";
	write_aa3d(event,detector,WWWDIR_DZ,name,"",false);

	STATUS("creating Event-display file with cut on DZ named "<<name<<std::endl);

	
	onlineEvents.clear();
	future = time::system_clock::now()+subRunDuration;
      } 
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineEventViewer " << endl);
  }
  
  return 0;
}

  
