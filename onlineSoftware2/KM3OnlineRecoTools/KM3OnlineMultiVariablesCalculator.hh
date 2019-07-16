#ifndef KM3ONLINE_MULTIVARIABLESCALCULATOR_INCLUDED
#define KM3ONLINE_MULTIVARIABLESCALCULATOR_INCLUDED

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <limits>

#include <boost/lexical_cast.hpp>

#include "JDetector/JModuleRouter.hh"
#include "JDetector/JPMTRouter.hh"
#include "JDetector/JPMTIdentifier.hh"
#include "JDetector/JPMT.hh"
#include "JDAQ/JDAQEvent.hh"
#include "JFit/JHistory.hh"
#include "JFit/JEvt.hh"
#include "JFit/JFitStatus.hh"
#include "JGeometry3D/JVector3D.hh"
#include "JGeometry3D/JAxis3D.hh"
#include "JLang/JSharedPointer.hh"

using namespace JPP;

namespace KM3ONLINE
{
  typedef std::numeric_limits<double> limits_t;

  class KM3OnlineMultiVariablesCalculator
  {

  private:

    JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> router_;
    
    double coc_;  // center of charge, the charge weighted DOM z position.
    double tot_;  // total charge
    double deltaPosZ_;
    
  public:

    KM3OnlineMultiVariablesCalculator()
    {
      coc_          = limits_t::quiet_NaN();
      tot_          = limits_t::quiet_NaN();
      deltaPosZ_    = limits_t::quiet_NaN();
    }
    
    KM3OnlineMultiVariablesCalculator(const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> &router):
      router_(router)
    {
      coc_          = limits_t::quiet_NaN();
      tot_          = limits_t::quiet_NaN();
      deltaPosZ_    = limits_t::quiet_NaN();
    }

    ~KM3OnlineMultiVariablesCalculator()
    {};


    // Calculators
  
    void CalculateDeltaPosZ(const std::vector<KM3NETDAQ::JDAQTriggeredHit> &hits,
			    const double quartile=0.2);

    template<class T>
    void CalculateHitsObservables(const std::vector<T> &hits);


    // Getters

    double getCoC() const {
      return coc_;
    }

    double getToT() const {
      return tot_;
    }

    double getDeltaPosZ() const {
      return deltaPosZ_;
    }

  };
}

void 
KM3ONLINE::KM3OnlineMultiVariablesCalculator::CalculateDeltaPosZ(const std::vector<KM3NETDAQ::JDAQTriggeredHit> &InHits,
								 const double quartile) 
{
  using KM3NETDAQ::JDAQTriggeredHit;

  std::vector<JDAQTriggeredHit> hits(InHits);
  std::sort(hits.begin(),hits.end(),
	    [](const JDAQTriggeredHit &lhs, const JDAQTriggeredHit &rhs) { return lhs.getT() < rhs.getT(); }
	    );

  if ( hits.empty() ){
    std::cout<<"Empty vector of hits" << std::endl;
    return;
  }

  const size_t n_quartile_hits=static_cast<size_t>(hits.size()*quartile);

  JGEOMETRY3D::JVector3D accumulated_pos(0,0,0);
  double accumulated_charge=0;

  JGEOMETRY3D::JVector3D raccumulated_pos(0,0,0);
  double raccumulated_charge=0;

  size_t n_hits=1;
  std::vector<JDAQTriggeredHit>::const_iterator hit=hits.begin();
  std::vector<JDAQTriggeredHit>::const_reverse_iterator rhit=hits.rbegin();
  for ( ; hit!=hits.end() ; hit++ , n_hits++){
    //forward hits
    const int om_id= hit->getModuleID();

    if (not router_->hasModule(om_id)) {
      const std::string str= boost::lexical_cast<std::string>(om_id);
      throw std::out_of_range("OM ID: "+str+" is not in JModuleRouter at: "+__PRETTY_FUNCTION__);
    }

    const JDETECTOR::JModule &module= router_->getModule(om_id);
    const JGEOMETRY3D::JVector3D &module_pos=module.getPosition();
    const double charge=static_cast<double>(hit->getToT());

    accumulated_pos += module_pos*charge;
    accumulated_charge+= charge;

    //backward hits
    const int rom_id= rhit->getModuleID();

    if (not router_->hasModule(rom_id)) {
      const std::string str= boost::lexical_cast<std::string>(rom_id);
      throw std::out_of_range("OM ID: "+str+" is not in JModuleRouter at: "+__PRETTY_FUNCTION__);
    }

    const JDETECTOR::JModule &rmodule= router_->getModule(rom_id);
    const JGEOMETRY3D::JVector3D &rmodule_pos= rmodule.getPosition();
    const double rcharge=static_cast<double>(rhit->getToT());

    raccumulated_pos += rmodule_pos*rcharge;
    raccumulated_charge+= rcharge;

    if (n_hits == n_quartile_hits) break;
    std::advance(rhit,1);
  }

  //fill deltaPos and check if this ok
  raccumulated_pos = raccumulated_pos/raccumulated_charge;
  accumulated_pos  = accumulated_pos/accumulated_charge;
 
  deltaPosZ_ = accumulated_pos.getZ()-raccumulated_pos.getZ();
}

/**
   calculates coc_, tot_
 */

template<class T>
void
KM3ONLINE::KM3OnlineMultiVariablesCalculator::CalculateHitsObservables(const std::vector<T> &hits) 
{
  double totTimesPosZ(0); // sum of charge times dom position z
  double tot(0); // total charge

  for (auto hit = hits.begin(); hit != hits.end(); ++hit){

    const int om_id= hit->getModuleID();

    if ( not router_->hasModule(om_id) ) {
      const std::string str= boost::lexical_cast<std::string>(om_id);
      throw std::out_of_range("OM ID: "+str+" is not in JModuleRouter at: "+__PRETTY_FUNCTION__);
    }
    const JDETECTOR::JModule &module= router_->getModule(om_id);
    const JGEOMETRY3D::JVector3D &module_pos=module.getPosition();

    totTimesPosZ += static_cast<double>( hit->getToT() ) * module_pos.getZ();
    tot  += static_cast<double>( hit->getToT() );
    
  }
 
  const double coc= totTimesPosZ/tot;
 
  coc_ = coc;
  tot_ = tot;
}

#endif
