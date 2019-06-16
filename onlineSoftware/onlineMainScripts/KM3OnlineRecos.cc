#include <iostream>
#include <memory>
#include <string>
#include <limits>
#include <exception>

#include "onlineeventmanager/KM3OnlineTags.hh"
#include "onlineeventmanager/KM3OnlineEvent.hh"
#include "onlineeventmanager/KM3OnlineGraphic.hh"
#include "onlineeventmanager/KM3OnlineTrack.hh"
#include "onlineeventmanager/KM3OnlineMultiVariables.hh"

#include "recotools/KM3OnlineLineFit.hh"
#include "recotools/KM3OnlineMultiVariablesCalculator.hh"
#include "recotools/KM3OnlineJRecos.hh"

#include "utilities/KM3OnlineToolKit.hh"

#include "JFit/JEvt.hh"
#include "JFit/JMuonParameters.hh"
#include "JFit/JFitApplications.hh"

#include "JDetector/JModuleRouter.hh"
#include "JDetector/JDetectorToolkit.hh"

#include "JNet/JLigierObjectIterator.hh"
#include "JNet/JControlHostObjectOutput.hh"
#include "JNet/JHostname.hh"

#include "JDAQ/JDAQEvent.hh"
#include "JDAQ/JDAQTags.hh"
#include "JDAQ/JDAQUTCExtended.hh"
#include "Jeep/JParser.hh"
#include "JLang/JSinglePointer.hh"
#include "JSupport/JSupport.hh"

int main(int argc, char **argv)
{
  using std::cout;
  using std::endl;
    
  using JFIT::JFit;
  using JFIT::JEvt;
  using JFIT::JFitApplication_t;
  using KM3NETDAQ::JDAQEvent;
  using JNET::JControlHostObjectOutput;

  using KM3ONLINE::KM3OnlineTrack;
  using KM3ONLINE::KM3OnlineEvent;
  using KM3ONLINE::KM3OnlineGraphic;

  namespace ot = KM3ONLINE::KM3ONLINETOOLKIT;

  JFIT::JMuonParameters muonParams;
  std::string DETECTORFILE;
  std::string PDFFILE;
  std::string AANetPDFFILE;
  
  int PORT;
  int BACKLOG;
  int debug;

  JNET::JHostname LISTENER;

  JPARSER::JParser<> zap("Online Script which runs the reconstruction applications, this will acts a server until some point.                         On the other hand; this script also will connect to a server to send few events");

  zap['a'] = make_field(DETECTORFILE)   ;
  zap['p'] = make_field(PDFFILE)        ;
  zap['A'] = make_field(AANetPDFFILE)   ;
  zap['@'] = make_field(muonParams)     = JPARSER::initialised();
  zap['q'] = make_field(BACKLOG)        = 1024;
  zap['P'] = make_field(PORT)           ;
  zap['L'] = make_field(LISTENER)       ;
  zap['d'] = make_field(debug)          ;

  zap.read(argc, argv);

  //load inputs detector file
  JDETECTOR::JDetector detector;
  JDETECTOR::load(DETECTORFILE, detector);
  const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter>
    moduleRouter(new JDETECTOR::JModuleRouter(detector));

  //******* recos
  //fast recos
  const KM3ONLINE::KM3OnlineLineFit lineFitReco(moduleRouter);
  KM3ONLINE::KM3OnlineMultiVariablesCalculator multiVariablesCalculator(moduleRouter);

  //Muon recos
  const KM3ONLINE::MuonRecoChain muonRecoChain(moduleRouter,muonParams,PDFFILE); 
  
  if (debug >= JEEP::debug_t) {
    setLongprint(cout);
  }

  //Prepare listener
  JLANG::JSinglePointer<JControlHostObjectOutput<KM3OnlineEvent> > listenerForOnline;
  JLANG::JSinglePointer<JControlHostObjectOutput<KM3OnlineGraphic> > listenerForGraphic; 

  const int timeOut(60);
  for (int n_trials = 1; n_trials <= timeOut; ++n_trials){ 
          
    DEBUG("Trying to connect to LISTENER, trial number: " <<n_trials<<endl);
    
    try {
      listenerForOnline.reset(new JControlHostObjectOutput<KM3OnlineEvent>(LISTENER));
      listenerForGraphic.reset(new JControlHostObjectOutput<KM3OnlineGraphic>(LISTENER));
      DEBUG("Succeeded Listener conection"<<endl);
      break;
    }
    catch(const std::exception& error) {
      sleep(1);
      DEBUG("Failed Listener conection"<<endl);
    }
   
    if ( n_trials==timeOut ){ 
      FATAL("Conection to LISTENER with hostname: "<<LISTENER.hostname<<" and port: "<<LISTENER.port<<
	    "exceeded time limit = "<<timeOut<<endl);
    }
  }
  // end listener preparation

  try {
    /* 
       Run the reconstruction application for all trigger events, this performs as a server
     */
    JNET::JLigierObjectIterator<JDAQEvent> in(PORT, BACKLOG);
    
    while ( in.hasNext() ){
      const JDAQEvent *daqEvent = in.next();
      
      const std::vector<KM3NETDAQ::JDAQTriggeredHit> &trigHits=
	daqEvent->getHits<KM3NETDAQ::JDAQTriggeredHit>();

      const std::vector<KM3NETDAQ::JDAQSnapshotHit > &snapHits=
	daqEvent->getHits<KM3NETDAQ::JDAQSnapshotHit>();

      multiVariablesCalculator.CalculateHitsObservables(trigHits);
      multiVariablesCalculator.CalculateDeltaPosZ(trigHits);       
      
      const double coc = multiVariablesCalculator.getCoC();
      const double tot = multiVariablesCalculator.getToT();
      const double charge_above =  multiVariablesCalculator.getToT();
      const double charge_below = multiVariablesCalculator.getChargeBelow();
      const double charge_ratio = multiVariablesCalculator.getChargeAbove();
      const double deltaPosZ    = multiVariablesCalculator.getDeltaPosZ();
      const unsigned n_trig_hits = trigHits.size();
      const unsigned n_snap_hits = snapHits.size();

      const KM3ONLINE::KM3OnlineMultiVariables multiVariables(coc,tot,charge_above,charge_below,
							      charge_ratio,deltaPosZ,n_trig_hits,n_snap_hits);

      const JFit &lineFit = lineFitReco.LineFitAngularReco(trigHits);
      
      JEvt gandalfs;
      muonRecoChain.getMuonRecoChain(*daqEvent,gandalfs);
      
      //Book Online things
      const int detectorID                              = daqEvent->getDetectorID();
      const int runNumber                               = daqEvent->getRunNumber();
      const int frameIndex                              = daqEvent->getFrameIndex();
      const KM3NETDAQ::JTriggerCounter_t triggerCounter = daqEvent->getCounter();
      const unsigned int timeSliceStartSec              = daqEvent->getTimesliceStart().getUTCseconds();

      KM3ONLINE::KM3OnlineEvent OnlineEvent(detectorID,runNumber,frameIndex,
					    triggerCounter,timeSliceStartSec,multiVariables);
      
      const KM3OnlineTrack &gandalf = ot::getFirstJFit(gandalfs,JFitApplication_t::JMUONGANDALF);
      OnlineEvent.SetRecoTrack(gandalf);
            
      DEBUG(OnlineEvent<<endl);

      /*
	End of reconstruction performing as a server
      */
      try                                 { listenerForOnline->put(OnlineEvent); }
      catch(const std::exception& error)  { ERROR(error.what()<<" at KM3OnlineRecos" << endl);          }      

      // Construct KM3OnlineGraphic

      const KM3NETDAQ::JDAQHeader &header = daqEvent->getDAQHeader(); 

      KM3OnlineGraphic graphicEvent(header,trigHits,snapHits,gandalf);

      try                                 { listenerForGraphic->put(graphicEvent); }
      catch(const std::exception& error)  { continue; }
      
    }
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error << endl);
  }
  
}

