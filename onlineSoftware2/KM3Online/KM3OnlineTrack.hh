#ifndef KM3ONLINETRACK_H_INCLUDED
#define KM3ONLINETRACK_H_INCLUDED

#include <cstdlib>

#include <JFit/JEvt.hh>
#include "JIO/JSTDIO.hh"

namespace KM3ONLINE 
{
  class KM3OnlineTrack
  {
    double x_;
    double y_;
    double z_;

    double dx_;
    double dy_;
    double dz_;

    double energy_;
    int type_;
    int status_;

    double quality_;
    double time_;
    int ndf_;
    
  public:

    /**
      Default constructor
    */
    KM3OnlineTrack():
      x_(0.0),y_(0.0),z_(0.0),
      dx_(0.0),dy_(0.0),dz_(0.0),
      energy_(0.0),type_(-1),status_(-1),
      quality_(0.0), time_(0.0), ndf_(-1)
    {}

    /**
      Parameterized  constructor
    */
    KM3OnlineTrack(const double x, const double y, const double z,
		   const double dx, const double dy, const double dz,
		   const double energy, const int type, const int status,
		   const double quality, const double time, const int ndf):
      x_(x),y_(y),z_(z),
      dx_(dx),dy_(dz),dz_(dz),
      energy_(energy),type_(type),status_(status),
      quality_(quality), time_(time), ndf_(ndf)
    {}

    /**
      Copy constructor
    */
    KM3OnlineTrack(const KM3OnlineTrack &rhs)
    {
      x_ = rhs.getX();
      y_ = rhs.getY();
      z_ = rhs.getZ();

      dx_ = rhs.getDX();
      dy_ = rhs.getDY();
      dz_ = rhs.getDZ();

      energy_  = rhs.getEnergy();
      quality_ = rhs.getQuality();
      time_    = rhs.getTime();

      type_   = rhs.getType();
      status_ = rhs.getStatus();
      ndf_    = rhs.getNDF();
    }

    /**
      Conversion constructor
    */
    KM3OnlineTrack(const JFIT::JFit &fit)
    {
      const JFIT::JHistory &history= fit.getHistory();
      if ( history.empty() ) std::out_of_range("Something wrong, JHistory is empty in "+std::string(__PRETTY_FUNCTION__));

      x_ = fit.getX();
      y_ = fit.getY();
      z_ = fit.getZ();

      dx_ = fit.getDX();
      dy_ = fit.getDY();
      dz_ = fit.getDZ();

      energy_  = fit.getE();
      quality_ = fit.getQ();
      time_    = fit.getT();

      type_   = history.back().type;
      status_ = fit.getStatus();
      ndf_    = fit.getNDF();
    }
    
    ~KM3OnlineTrack(){}

    double getX()       const { return x_;      } 
    
    double getY()       const { return y_;      } 
    
    double getZ()       const { return z_;      } 

    double getDX()      const { return dx_;     } 

    double getDY()      const { return dy_;     } 

    double getDZ()      const { return dz_;     } 

    double getEnergy()  const { return energy_;  } 

    double getQuality() const { return quality_; } 

    double getTime()    const { return time_;    } 
 
    int getType()       const { return type_;    } 

    int getStatus()     const { return status_;  } 

    int getNDF()        const { return ndf_;     } 


    static int sizeOf()
    {
      std::size_t n_doubles=9;
      std::size_t n_ints=3;
      return (sizeof(double)*n_doubles + sizeof(int)*n_ints);
    }


    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineTrack& track)
    {
      in >> track.x_;
      in >> track.y_;
      in >> track.z_;
      in >> track.dx_;
      in >> track.dy_;
      in >> track.dz_;
      in >> track.energy_;
      in >> track.quality_;
      in >> track.time_;
      in >> track.type_;
      in >> track.status_;
      in >> track.ndf_;
      
      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineTrack& track)
    {
      out << track.x_;
      out << track.y_;
      out << track.z_;
      out << track.dx_;
      out << track.dy_;
      out << track.dz_;
      out << track.energy_;
      out << track.quality_;
      out << track.time_;
      out << track.type_;
      out << track.status_;
      out << track.ndf_;

      return out;
    }


    friend inline std::ostream& operator<<(std::ostream &out,
                                           const KM3OnlineTrack &track)
    {
      using std::endl;

      out << " KM3OnlineTrack: "                                << endl;
      out << "X       :   " << FIXED(7,2) << track.getX()       << endl;
      out << "Y       :   " << FIXED(7,2) << track.getY()       << endl;
      out << "Z       :   " << FIXED(7,2) << track.getZ()       << endl;
      out << "DX      :   " << FIXED(7,3) << track.getDX()      << endl;
      out << "DY      :   " << FIXED(7,3) << track.getDY()      << endl;
      out << "DZ      :   " << FIXED(7,3) << track.getDZ()      << endl;
      out << "Energy  :   " << FIXED(7,3) << track.getEnergy()  << endl;
      out << "Quality :   " << FIXED(7,3) << track.getQuality() << endl;
      out << "Time    :   " << FIXED(7,3) << track.getTime()    << endl;
      out << "Type    :   " << FIXED(7,3) << track.getType()    << endl;
      out << "Status  :   " << FIXED(7,3) << track.getStatus()  << endl;
      out << "NDF     :   " << FIXED(7,3) << track.getNDF()     << endl;

      return out;
    }

  };

}

#endif
