#ifndef __KM3ONLINEMUONRECOS__
#define __KM3ONLINEMUONRECOS__

#include "JDAQ/JDAQEvent.hh"

#include "JFit/JEvt.hh"
#include "JFit/JHistory.hh"
#include "JFit/JFitStatus.hh"
#include "JFit/JFitApplications.hh"

#include "JFit/JMuonParameters.hh"
#include "JFit/JMuonPrefit.hh"
#include "JFit/JMuonSimplex.hh"
#include "JFit/JMuonGandalf.hh"

#include "JFit/JShowerParameters.hh"
#include "JFit/JORCAShowerPrefit.hh"
#include "JFit/JORCAShowerPositionFit.hh"
#include "JFit/JORCAShowerFit.hh"

namespace KM3ONLINE
{
  class MuonRecoChain
  {
    JFIT::JMuonPrefit  prefit_;
    JFIT::JMuonSimplex simplex_;
    JFIT::JMuonGandalf gandalf_;

    JFIT::JMuonPrefitParameters_t prefitParams_;

  public:
    MuonRecoChain(const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> &router,
		  const JFIT::JMuonParameters &parameters,
		  const std::string pdfFile)
    {
      prefit_  = {router,parameters.prefit};
      simplex_ = {router,parameters.simplex};
      gandalf_ = {router,parameters.gandalf,pdfFile};

      prefitParams_ = {parameters.prefit};
    }

    void 
    getMuonRecoChain(const KM3NETDAQ::JDAQEvent &daqEvent,
		     JFIT::JEvt &OutFits) const
    {
      const KM3NETDAQ::JDAQTimeslice timeSlice(daqEvent,true);
      
      JFIT::JEvt prefits;
      const KM3NETDAQ::JDAQTimeslice timeSliceForPrefit(daqEvent,!prefitParams_.useL0);
      prefit_.getJEvt(timeSlice,timeSliceForPrefit,prefits);

      JFIT::JEvt simplexs;
      simplex_.getJEvt(timeSlice, prefits, simplexs);

      gandalf_.getJEvt(timeSlice,simplexs, OutFits);
    }

  };

  class ShowerRecoChain
  {
    JFIT::JORCAShowerPrefit  prefit_;
    JFIT::JORCAShowerPositionFit positionFit_;
    JFIT::JORCAShowerFit finalFit_;

    JFIT::JShowerPrefitParameters_t prefitParams_;

  public:
    ShowerRecoChain(const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> &router,
		    const JFIT::JShowerParameters &parameters,
		    const std::string pdfFilePosFit,
		    const std::string pdfFileFit)
    {
      prefit_      = {router,parameters.prefit};
      positionFit_ = {router,parameters.positionfit,pdfFilePosFit};
      finalFit_    = {router,parameters.showerfit,pdfFileFit};

      prefitParams_ = {parameters.prefit};
    }

    void 
    getShowerRecoChain(const KM3NETDAQ::JDAQEvent &daqEvent,
		       JFIT::JEvt &OutFits) const
    {
      const KM3NETDAQ::JDAQTimeslice timeSlice(daqEvent,true);

      JFIT::JEvt prefits;
      const KM3NETDAQ::JDAQTimeslice timeSliceForPrefit(daqEvent,!prefitParams_.useL0);
      prefit_.getJEvt(timeSlice,timeSliceForPrefit,prefits);
		  
      JFIT::JEvt positionFits;
      const KM3NETDAQ::JDAQTimeslice timeSliceBuildL2(daqEvent,false);
      positionFit_.getJEvt(timeSlice,timeSliceBuildL2,prefits,positionFits);    

      finalFit_.getJEvt(timeSlice,positionFits,OutFits);
    }

  };

}

#endif
