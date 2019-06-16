#ifndef __KM3ONLINEVENT__
#define __KM3ONLINEVENT__

#include <ostream>
#include <map>
#include <string>
#include <stdexcept>

#include "KM3OnlineDataTypes.hh"

#include "JLang/JType.hh"
#include "JDAQ/JDAQEvent.hh"
#include "JDAQ/JDAQPreamble.hh"
#include "JIO/JSTDIO.hh"

#include "KM3OnlineTrack.hh"
#include "KM3OnlineMultiVariables.hh"


namespace KM3ONLINE
{
  using KM3NETDAQ::JDAQType;

  class KM3OnlineEvent:
    public KM3NETDAQ::JDAQPreamble 
  {
  private:

    int detectorID_;
    int runNumber_;
    int frameIndex_;
    KM3NETDAQ::JTriggerCounter_t triggerCounter_; 
    unsigned int timeSliceStartSec_;
    KM3OnlineMultiVariables multiVar_;    

    KM3OnlineTrack jgandalf_;
    KM3OnlineTrack shower_;
     
  public:
   
    KM3OnlineEvent() :
      KM3NETDAQ::JDAQPreamble(JDAQType<KM3OnlineEvent>())
    {}
    
    KM3OnlineEvent(const int detectorID, 
		   const int runNumber, 
		   const int frameIndex,
		   const KM3NETDAQ::JTriggerCounter_t triggerCounter, 
		   const unsigned int timeSliceStartSec,
		   const KM3OnlineMultiVariables &multiVar):
      KM3NETDAQ::JDAQPreamble(JDAQType<KM3OnlineEvent>()),
      detectorID_(detectorID),
      runNumber_(runNumber),
      frameIndex_(frameIndex),
      triggerCounter_(triggerCounter),
      timeSliceStartSec_(timeSliceStartSec),
      multiVar_(multiVar)
    {}
        
    ~KM3OnlineEvent()
    {}

    // Setters

    void SetRecoShower(const KM3OnlineTrack &fit)    { shower_   = fit; } 

    void SetRecoTrack(const KM3OnlineTrack &fit)     { jgandalf_ = fit; } 

    // Getters
      
    KM3OnlineTrack GetRecoShower()                   const { return shower_;      }

    KM3OnlineTrack GetRecoTrack()                    const { return jgandalf_;    }

    int GetDetectorID()                              const { return detectorID_;  } 
        
    int GetRunNumber()                               const { return runNumber_;   }
        
    int GetFrameIndex()                              const { return frameIndex_;  }
                   
    KM3NETDAQ::JTriggerCounter_t GetTriggerCounter() const { return triggerCounter_;    }
        
    unsigned int GetUTCTimeSliceStartSec()           const { return timeSliceStartSec_; }
    
    KM3OnlineMultiVariables GetMultiVariables()      const { return multiVar_;          }

    // size of the object for data stream comunication

    int getSize() const
    {
      const std::size_t n_tracks=2;
      const std::size_t n_ints=3;

      return (KM3NETDAQ::JDAQPreamble::sizeOf()           +
	      KM3OnlineMultiVariables::sizeOf()           + 
	      KM3OnlineTrack::sizeOf()*n_tracks           +
	      sizeof(int)*n_ints + sizeof(unsigned int)   +
	      sizeof(KM3NETDAQ::JTriggerCounter_t)        );
    }


    // IO operations

    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineEvent& event)
    {
      in >> static_cast<KM3NETDAQ::JDAQPreamble&>   (event);
      in >> event.detectorID_;
      in >> event.runNumber_;
      in >> event.frameIndex_;
      in >> event.triggerCounter_;
      in >> event.timeSliceStartSec_;
      in >> event.shower_;
      in >> event.jgandalf_;
      in >> event.multiVar_;
           
      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineEvent& event)
    {
      out << static_cast<const KM3NETDAQ::JDAQPreamble&>   (event);
      out << event.detectorID_;
      out << event.runNumber_;
      out << event.frameIndex_;
      out << event.triggerCounter_;
      out << event.timeSliceStartSec_;
      out << event.shower_;
      out << event.jgandalf_;
      out << event.multiVar_;
     
      return out;
    }

    
    friend inline std::ostream& operator<<(std::ostream& os, const KM3OnlineEvent& event) 
					   
    {
      using std::endl;

      os << "*******************" << endl;
      os << "   KM3OnlineEvent: " << endl;
      os << "*******************" << endl;
      
      os<< "***DetectorID    : "<<event.GetDetectorID()          << endl;
      os<< "***RunNumber     : "<<event.GetRunNumber()           << endl;
      os<< "***FrameIndex    : "<<event.GetFrameIndex()          << endl;
      os<< "***TriggerCoser  : "<<event.GetTriggerCounter()      << endl;  
      os<< "***UTCseconds    : "<<event.GetUTCTimeSliceStartSec()<< endl;
                
      os << "***JGandalf: "<< endl;
      os << event.GetRecoTrack();

      os << "***Shower: "<< endl;
      os << event.GetRecoShower();

      os << event.GetMultiVariables();
      
      return os;
    }

    ClassDef(KM3OnlineEvent,1);
  };

}

#endif

