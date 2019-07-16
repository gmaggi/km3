#ifndef KM3ONLINE_SCORED_EVENT_INCLUDED
#define KM3ONLINE_SCORED_EVENT_INCLUDED

#include <ostream>
#include <map>
#include <string>
#include <stdexcept>

#include "KM3OnlineDataTypes.hh"

#include "JLang/JType.hh"
#include "JDAQ/JDAQEvent.hh"
#include "JDAQ/JDAQPreamble.hh"
#include "JIO/JSTDIO.hh"

#include "KM3OnlineEvent.hh"

namespace KM3ONLINE
{
  using KM3NETDAQ::JDAQType;

  class KM3OnlineScoredEvent:
    public KM3OnlineEvent
  {   

  private:    
 
    double score_;
  
  public:
    
    KM3OnlineScoredEvent() :
      KM3OnlineEvent(),
      score_( std::numeric_limits<double>::quiet_NaN() )
    {
      this->type   = this->getDataType();
      this->length = 0; 
    }
    
    KM3OnlineScoredEvent(const KM3OnlineEvent &onlineEvent,
			 const double score):
      KM3OnlineEvent(onlineEvent),
      score_(score)
    {}
        
    virtual ~KM3OnlineScoredEvent()
    {}
   

    double getScore() const { return score_; }
    
    // size of the object for data stream comunication

    int getSize() const override
    {
      return ( JDAQPreamble::sizeOf()                                                                                 +
               JDAQHeader::sizeOf()                                                                                   +
               sizeof(int) + JDAQKeyHit::sizeOf()*static_cast<const vector<JDAQKeyHit>&>(*this).size()                +
               sizeof(int) + JDAQTriggeredHit::sizeOf()*static_cast<const vector<JDAQTriggeredHit>&>(*this).size()    +
               KM3OnlineTrack::sizeOf()                                                                               +
               KM3OnlineMultiVariables::sizeOf()                                                                      +
	       sizeof(double)                                                                                         );
    }
    
    // IO operations
    
    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineScoredEvent& event)
    {
      in >> static_cast<KM3OnlineEvent&> (event);
      in >> event.score_ ;
                 
      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineScoredEvent& event)
    {
      out << static_cast<const KM3OnlineEvent&> (event);
      out << event.score_ ;      

      return out;
    }
    
    
    friend inline std::ostream& operator<<(std::ostream& os, const KM3OnlineScoredEvent& event) 
					   
    {
      using std::endl;

      os << "*************************" << endl;
      os << "   KM3OnlineScoredEvent: " << endl;
      os << "*************************" << endl;
      
      os<< "Event score : "<<event.score_                        << endl;
      os<< static_cast<const KM3ONLINE::KM3OnlineEvent&> (event) << endl;      
      
      return os;
    }
   
    ClassDef(KM3OnlineScoredEvent,1);
  };

}

#endif

