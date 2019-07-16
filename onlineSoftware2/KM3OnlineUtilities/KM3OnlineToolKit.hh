#ifndef __KM3ONLINETOOLKIT__
#define __KM3ONLINETOOLKIT__

#include "JFit/JEvt.hh"
#include "JFit/JHistory.hh"
#include "JFit/JFitApplications.hh"

#include "KM3Online/KM3OnlineTrack.hh"

namespace KM3ONLINE
{
  namespace KM3ONLINETOOLKIT
  {
    /*
       This gives the best fit, assuming that the parameter fits is ordered in quality
    */ 
    KM3ONLINE::KM3OnlineTrack getFirstJFit(const JFIT::JEvt &fits, 
					   const JFIT::JFitApplication_t fitType)
    {
      KM3ONLINE::KM3OnlineTrack out;    
      
      for (JFIT::JEvt::const_iterator fit = fits.begin(); fit != fits.end(); ++fit){
	if ( fit->getHistory().back().type == fitType ){ 
	  KM3ONLINE::KM3OnlineTrack temp(*fit);
	  out=temp;
	  break;
	} 
      }

      return out;
    }
    
  }
}
#endif
