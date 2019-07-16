#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <stdexcept>

#include "TTimeStamp.h"
#include "TFile.h"

#include "evt/Head.hh"
#include "evt/Evt.hh"

#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"

#include "KM3Online/KM3OnlineTags.hh"
#include "KM3Online/KM3OnlineEvent.hh"
#include "KM3Online/KM3OnlineTrack.hh"
#include "KM3Online/KM3OnlineMultiVariables.hh"

#include "KM3OnlineUtilities/KM3OnlineFileWriter.hh"


int main(int argc, char **argv)
{
  JPARSER::JParser<> zap("KM3OnlineFileWriter");
  
  JNET::JHostname HOSTNAME;
  std::string MAINDIR;
  std::string DETECTOR;
  int debug;

  zap['H'] = make_field(HOSTNAME)  ;
  zap['f'] = make_field(MAINDIR)   ;
  zap['D'] = make_field(DETECTOR)  ;
  zap['d'] = make_field(debug)     = 1;

  zap.read(argc, argv);

  if ( DETECTOR!="ARCA" and DETECTOR!="ORCA" ) {
    throw std::invalid_argument("Wrong DETECTOR, should be ARCA or ORCA, you passed me: "+DETECTOR);
  }

  KM3ONLINE::KM3OnlineFileWriter fileWriter(MAINDIR,DETECTOR);

  JSUPPORT::JMeta meta(argc,argv);
  {
    TFile* file = TFile::Open("dummy_meta.root", "recreate");

    putObject(file, meta);

    file->Write();
    file->Close();
  }

  try{
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent> data(HOSTNAME);

    std::vector<Evt> dumpedEvents;

    int detectorID(0);
    int runNumber(0);

    while ( data.hasNext() ){

      const KM3ONLINE::KM3OnlineEvent *in = data.next();
   
      Evt out;

      const KM3ONLINE::KM3OnlineTrack &olTrack  = in->getTrack();

      if ( olTrack.getStatus()!=1 ) continue;

      std::vector<Trk> trks;
      fileWriter.make_Trk( olTrack,trks  ); //<- the order matters for the trks.id's
      
      out.trks            = trks;
      out.det_id          = in->getDetectorID();
      out.run_id          = in->getRunNumber();
      out.frame_index     = in->getFrameIndex();
      out.t.Set( in->getTimesliceStart().getUTCseconds(), true, 0, false );

      const KM3ONLINE::KM3OnlineMultiVariables &multiVar = in->getMultiVariables();

      out.setusr("CoC",         multiVar.getCoC()                    );
      out.setusr("ToT",         multiVar.getToT()                    );
      out.setusr("ChargeAbove", multiVar.getChargeAbove()            );
      out.setusr("ChargeBelow", multiVar.getChargeBelow()            );
      out.setusr("ChargeRatio", multiVar.getChargeRatio()            );
      out.setusr("DeltaPosZ",   multiVar.getDeltaPosZ()              );
      out.setusr("NSnapHits",   (double)multiVar.getNSnapHits()      );
      out.setusr("NTrigHits",   (double)multiVar.getNTriggeredHits() );

      dumpedEvents.push_back(out);

      Evt lastEvt;
      if ( not dumpedEvents.empty() ) { lastEvt = dumpedEvents.back(); } 

      if ( (in->getDetectorID() > detectorID  or 
	    in->getRunNumber()  > runNumber) ) {
	
	if ( detectorID==0 and runNumber==0 ){ 
	  detectorID = in->getDetectorID();
	  runNumber  = in->getRunNumber();
	  continue;
	}

	if ( not dumpedEvents.empty() ) { dumpedEvents.pop_back(); }//<-remove this event with another run/detID number 

	const std::string &fileName = fileWriter.makeDirectory( detectorID,runNumber);
	fileWriter.writeFile(dumpedEvents,meta);

	const std::time_t &writingTime = std::chrono::system_clock::to_time_t( chrono::system_clock::now() );
	std::cout<<"File "<<fileName<<" was created with "<<dumpedEvents.size()<<\
	  " events, at "<<std::ctime(&writingTime)<<std::endl;

	detectorID = in->getDetectorID();
	runNumber  = in->getRunNumber();

	dumpedEvents.clear();
	dumpedEvents.push_back(lastEvt);//<-put back the removed event
      }

      DEBUG(out<<std::endl);
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineFileWriter " << endl);
  }
      
  return 0;
}

