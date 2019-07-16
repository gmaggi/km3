#ifndef KM3ONLINE_LINEFIT_INCLUDED
#define KM3ONLINE_LINEFIT_INCLUDED

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <iterator>
#include <numeric>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "JDetector/JModuleRouter.hh"
#include "JDAQ/JDAQEvent.hh"
#include "JFit/JHistory.hh"
#include "JFit/JEvt.hh"
#include "JFit/JFitStatus.hh"
#include "JGeometry3D/JVector3D.hh"
#include "JLang/JSharedPointer.hh"

namespace KM3ONLINE
{ 
  class KM3OnlineLineFit
  {
    
  private:
    
    JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> router_;
    
  public:
    
    KM3OnlineLineFit(const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> &router):
      router_(router)
    {};
    
    ~KM3OnlineLineFit()
    {};
        
    template <class T>  
    static bool EqualIdDoms(const T &hit1,
			    const T &hit2)
    {
      return hit1.getModuleID() == hit2.getModuleID();
    }
    
    template <class T>
    JFIT::JFit
    LineFitAngularReco(const std::vector<T> &hits,
		       const size_t min_n_doms=2) const;
              
  };
  
}
  
/**
   LineFit angular reconstruction algorithm is obtained from the IceCube.
   
   If the event does not fulfil the LineFit track conditions, a track is gonna be put anyhow
   in the output event at the moment of calling this function. nonetheless, the event 
   can be skipped by selecting status ==1 (this is a good fit)   
*/

template <class T>
JFIT::JFit
KM3ONLINE::KM3OnlineLineFit::LineFitAngularReco(const std::vector<T> &hits,
						const size_t min_n_doms) const
{
  const JFIT::JHistory history(JFIT::JLINEFIT);
  const double energy(0);

  JFIT::JFit track = {history,
		      0,0,0,//pos
		      0,0,0,//dir
		      0,-1,-1,energy,//time,quality, NDF, E
		      JFIT::JFitStatus_t::ERROR};
  

  if (hits.empty()){
    std::cout<<"Empty vector of hits"<<std::endl;
    track={history, 0,0,0, 0,0,0, 0,-1,-1,energy, JFIT::JFitStatus_t::INSUFFICIENT_NUMBER_OF_HITS};    

    return track;
  }

  std::vector<T> cp_hits(hits);
  cp_hits.erase( std::unique(cp_hits.begin(),cp_hits.end(),EqualIdDoms<T>),  
		 cp_hits.end());
  
  if (cp_hits.size() < min_n_doms){
    std::cout<<"Minimun number of Doms is: "<<min_n_doms<<std::endl;
    std::cout<<", but I got: "<<cp_hits.size()<<", No reco track then"<<std::endl;
    track={history, 0,0,0, 0,0,0, 0,-1,-1,energy, JFIT::JFitStatus_t::INSUFFICIENT_NUMBER_OF_DOMS};    

    return track;
  }

  double avTime=0.0;
  double avTimeSquared=0.0;
  double ampsum=0.0;
     
  JGEOMETRY3D::JVector3D avPos(0,0,0);
  JGEOMETRY3D::JVector3D avTP(0,0,0);

  for (auto hit= hits.begin(); hit != hits.end(); hit++){
    const int om_id= hit->getModuleID();
    
    if (not router_->hasModule(om_id)) {
      const std::string str= boost::lexical_cast<std::string>(om_id);
      throw std::out_of_range("OM ID: "+str+" is not in JModuleRouter at: "+__PRETTY_FUNCTION__);
    }
    
    const JDETECTOR::JModule &module= router_->getModule(om_id);
    const JGEOMETRY3D::JVector3D &module_pos=module.getPosition();       
        
    const double amp=static_cast<double>(hit->getToT());
    const double time = hit->getT();
    
    avTime += amp*time;
    avTimeSquared += amp*time*time;
    ampsum+=amp;
    
    avPos+= module_pos*amp;
    avTP+= module_pos*amp*time; 
  }

  if (avTimeSquared == avTime*avTime) {
    std::cout<<"Reco will not converge"<<std::endl;
    return track;
  }

  avTime /= ampsum;
  avTimeSquared /= ampsum;
  avPos/= ampsum;
  avTP /= ampsum;

  const JGEOMETRY3D::JVector3D &velocity= 
    (avTP-avPos*avTime)/(avTimeSquared - avTime*avTime);

  const double speed=velocity.getLengthSquared();  

  if (speed <= 0 ) { 
    std::cout<<"speed: "<<speed<<" is not positive "<<std::endl;
    return track;
  }   
  
  const double sqrt_speed= std::sqrt(speed);
  const double x_dir=velocity.getX()/sqrt_speed;
  const double y_dir=velocity.getY()/sqrt_speed;
  const double z_dir=velocity.getZ()/sqrt_speed;
  const int ndf = static_cast<int>(hits.size());

  track = {history,
	   avPos.getX(),avPos.getY(),avPos.getZ(),
	   x_dir,y_dir,z_dir,
	   avTime,sqrt_speed,ndf,energy,
	   JFIT::JFitStatus_t::OKAY};
  
  return track;
}

#endif
