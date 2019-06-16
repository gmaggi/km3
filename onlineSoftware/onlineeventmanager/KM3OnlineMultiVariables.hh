#ifndef KM3ONLINE_MULTIVARIABLES_INCLUDED
#define KM3ONLINE_MULTIVARIABLES_INCLUDED

#include <string>
#include <iostream>

#include "JIO/JSerialisable.hh"


namespace KM3ONLINE
{
  class KM3OnlineMultiVariables
  {

  private:
    
    double coc_;  
    double tot_;  
    double charge_above_; 
    double charge_below_; 
    double charge_ratio_; 
    double deltaPosZ_;
    unsigned n_trig_hits_;
    unsigned n_snap_hits_;
    
  public:

    KM3OnlineMultiVariables():
      coc_( std::numeric_limits<double>::quiet_NaN() ),
      tot_( std::numeric_limits<double>::quiet_NaN() ),
      charge_above_( std::numeric_limits<double>::quiet_NaN() ),
      charge_below_( std::numeric_limits<double>::quiet_NaN() ),
      charge_ratio_( std::numeric_limits<double>::quiet_NaN() ),
      deltaPosZ_( std::numeric_limits<double>::quiet_NaN() ),
      n_trig_hits_(0),
      n_snap_hits_(0)
    {}

    
    KM3OnlineMultiVariables(const double coc,
			    const double tot,
			    const double charge_above,
			    const double charge_below,
			    const double charge_ratio,
			    const double deltaPosZ,
			    const unsigned n_trig_hits,
			    const unsigned n_snap_hits):
      coc_(coc),
      tot_(tot),
      charge_above_(charge_above),
      charge_below_(charge_below),
      charge_ratio_(charge_ratio),
      deltaPosZ_(deltaPosZ),
      n_trig_hits_(n_trig_hits),
      n_snap_hits_(n_snap_hits)
    {}

    ~KM3OnlineMultiVariables()
    {};

    
    double getCoC() const {
      return coc_;
    }

    double getToT() const {
      return tot_;
    }

    double getChargeAbove() const {
      return charge_above_;
    }

    double getChargeBelow() const {
      return charge_below_;
    }

    double getChargeRatio() const {
      return charge_ratio_;
    }

    double getDeltaPosZ() const {
      return deltaPosZ_;
    }

    unsigned getNTriggeredHits() const {
      return n_trig_hits_;
    }

    unsigned getNSnapHits() const {
      return n_snap_hits_;
    }

    // Size used for data stream comunication

    static int sizeOf()
    {
      const int n_doubles =6;
      const int n_unsigned  =2;
      
      return ( sizeof(double)*n_doubles +
	       sizeof(unsigned)*n_unsigned );
    }


    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineMultiVariables& multVar)
    {
      in >> multVar.coc_;
      in >> multVar.tot_;
      in >> multVar.charge_above_;
      in >> multVar.charge_below_;
      in >> multVar.charge_ratio_;
      in >> multVar.deltaPosZ_;
      in >> multVar.n_trig_hits_;
      in >> multVar.n_snap_hits_;

      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineMultiVariables &multVar)
    {
      out << multVar.coc_;
      out << multVar.tot_;
      out << multVar.charge_above_;
      out << multVar.charge_below_;
      out << multVar.charge_ratio_;
      out << multVar.deltaPosZ_;
      out << multVar.n_trig_hits_;
      out << multVar.n_snap_hits_;

      return out;
    }

    friend inline std::ostream& operator<<( std::ostream &os, const KM3OnlineMultiVariables &multVar)
    {
      using std::endl;

      os << "****************************" << endl;
      os << "   KM3OnlineMultiVariables: " << endl;
      os << "****************************" << endl;

      os<< "***CoC             : "<<multVar.coc_          << endl;
      os<< "***ToT             : "<<multVar.tot_          << endl;
      os<< "***Charge Above    : "<<multVar.charge_above_ << endl;
      os<< "***Charge Below    : "<<multVar.charge_below_ << endl;
      os<< "***Charge Ratio    : "<<multVar.charge_ratio_ << endl;
      os<< "***Delta Pos Z     : "<<multVar.deltaPosZ_    << endl;
      os<< "***N Trigger Hits  : "<<multVar.n_trig_hits_  << endl;
      os<< "***N Snap Hits     : "<<multVar.n_snap_hits_  << endl;

      return os;
    } 

  };
}
#endif
