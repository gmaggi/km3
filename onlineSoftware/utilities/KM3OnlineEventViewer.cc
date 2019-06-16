#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

#include <onlineeventmanager/KM3OnlineDataTypes.hh>

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

#include <onlineeventmanager/KM3OnlineGraphic.hh>
#include <onlineeventmanager/KM3OnlineEvent.hh>
#include <onlineeventmanager/KM3OnlineTrack.hh>
#include <onlineeventmanager/KM3OnlineTags.hh>


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
  using KM3ONLINE::KM3OnlineGraphic;
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
  std::vector<KM3OnlineGraphic> graphics;

  try{
    JNET::JControlHostObjectIterator<KM3OnlineGraphic> data(HOSTNAME);

    while ( data.hasNext() ){

      const KM3OnlineGraphic *graphic = data.next();
      DEBUG(*graphic<< std::endl);

      graphics.push_back(*graphic);

      if ( time::system_clock::now() >= future ){
	
	std::vector<KM3OnlineGraphic>::const_iterator selectedEvent=
          std::max_element( graphics.begin(), graphics.end(),
			    [] (const KM3OnlineGraphic &lhs, const KM3OnlineGraphic &rhs)
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

	const KM3NETDAQ::JDAQHeader &header = graphic->getDAQHeader();
	const int run = header.getRunNumber();
	
	const std::string label = std::to_string(run)+"."+buffer;  
	const std::string name = "Run"+label+".js.gz";
	write_aa3d(event,detector,WWWDIR_DZ,name,"",false);

	STATUS("creating Event-display file with cut on DZ named "<<name<<std::endl);

	
	graphics.clear();
	future = time::system_clock::now()+subRunDuration;
      } 
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineEventViewer " << endl);
  }
  
  return 0;
}

  
