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

#include "onlineeventmanager/KM3OnlineTags.hh"
#include "onlineeventmanager/KM3OnlineEvent.hh"
#include "onlineeventmanager/KM3OnlineTrack.hh"
#include "onlineeventmanager/KM3OnlineMultiVariables.hh"

#include "utilities/KM3OnlineFileWriter.hh"


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

      const KM3ONLINE::KM3OnlineTrack &olTrack  = in->GetRecoTrack();
      const KM3ONLINE::KM3OnlineTrack &olShower = in->GetRecoShower();

      if ( olTrack.getStatus()!=1 and olShower.getStatus()!=1 ) continue;

      std::vector<Trk> trks;
      fileWriter.make_Trk( olTrack,trks  ); //<- the order matters for the trks.id's
      fileWriter.make_Trk( olShower,trks );

      out.trks            = trks;
      out.det_id          = in->GetDetectorID();
      out.run_id          = in->GetRunNumber();
      out.frame_index     = in->GetFrameIndex();
      out.trigger_counter = in->GetTriggerCounter();
      out.t.Set( in->GetUTCTimeSliceStartSec(), true, 0, false );

      const KM3ONLINE::KM3OnlineMultiVariables &multiVar = in->GetMultiVariables();

      out.setusr("CoC",         multiVar.getCoC()                    );
      out.setusr("ToT",         multiVar.getToT()                    );
      out.setusr("ChargeAbove", multiVar.getChargeAbove()            );
      out.setusr("ChargeBelow", multiVar.getChargeBelow()            );
      out.setusr("ChargeRatio", multiVar.getChargeRatio()            );
      out.setusr("DeltaPosZ",   multiVar.getDeltaPosZ()              );
      out.setusr("NSnapHits",   (double)multiVar.getNSnapHits()      );
      out.setusr("NTrigHits",   (double)multiVar.getNTriggeredHits() );

      dumpedEvents.push_back(out);

      if ( (in->GetDetectorID() > detectorID  or 
	    in->GetRunNumber()  > runNumber) ) {
	
	if ( detectorID==0 and runNumber==0 ){ 
	  detectorID = in->GetDetectorID();
	  runNumber  = in->GetRunNumber();
	  continue;
	}

	const std::string &fileName = fileWriter.makeDirectory( detectorID,runNumber);
	fileWriter.writeFile(dumpedEvents,meta);

	const std::time_t &writingTime = std::chrono::system_clock::to_time_t( chrono::system_clock::now() );
	std::cout<<"File "<<fileName<<" was created with "<<dumpedEvents.size()<<\
	  " events, at "<<std::ctime(&writingTime)<<std::endl;

	detectorID = in->GetDetectorID();
	runNumber  = in->GetRunNumber();

	dumpedEvents.clear();
      }

      DEBUG(out<<std::endl);
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineFileWriter " << endl);
  }
      
  return 0;
}

