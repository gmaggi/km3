#ifndef KM3ONLINE_EVENT_INCLUDED
#define KM3ONLINE_EVENT_INCLUDED

#include <ostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "KM3OnlineDataTypes.hh"

#include "JIO/JSTDIO.hh"
#include "JLang/JType.hh"

#include "JDAQ/JDAQPreamble.hh"
#include "JDAQ/JDAQHeader.hh"
#include "JDAQ/JDAQKeyHit.hh"
#include "JDAQ/JDAQTriggeredHit.hh"

#include "KM3OnlineTrack.hh"
#include "KM3OnlineMultiVariables.hh"

namespace KM3ONLINE
{
  using std::vector;

  using KM3NETDAQ::JDAQEvent;
  using KM3NETDAQ::JDAQPreamble;
  using KM3NETDAQ::JDAQHeader;
  using KM3NETDAQ::JDAQKeyHit;
  using KM3NETDAQ::JDAQTriggeredHit;
    
  class KM3OnlineEvent:
    public JDAQPreamble,
    public JDAQHeader,
    public vector<JDAQTriggeredHit>,
    public vector<JDAQKeyHit>
  {
  private:

    KM3OnlineTrack track_;
    KM3OnlineMultiVariables multiVariables_;    

  public:    
      
    KM3OnlineEvent() :
      JDAQPreamble(KM3NETDAQ::JDAQType<KM3OnlineEvent>()),
      JDAQHeader(),
      vector<JDAQTriggeredHit>(),
      vector<JDAQKeyHit>(),
      multiVariables_()
    {} 
    
    KM3OnlineEvent(const JDAQHeader &header, 
		   const vector<KM3NETDAQ::JDAQTriggeredHit> &trigHits, 
		   const vector<KM3NETDAQ::JDAQKeyHit> &keyHits, 
		   const KM3OnlineMultiVariables &multiVariables):
      JDAQPreamble(KM3NETDAQ::JDAQType<KM3OnlineEvent>()),
      JDAQHeader(header),
      vector<JDAQTriggeredHit>(trigHits),
      vector<JDAQKeyHit>(keyHits),
      multiVariables_(multiVariables)
    {}
         
    ~KM3OnlineEvent()
    {}

    KM3OnlineTrack getTrack() const
    {
      return track_;
    }

    void setTrack(const KM3OnlineTrack &track) 
    {
      track_ = track;
    }

    JDAQHeader getDAQHeader() const
    {
      return static_cast<const JDAQHeader&>(*this);
    } 

    KM3OnlineMultiVariables getMultiVariables() const 
    {
      return multiVariables_;
    }

    template<class T>
    const vector<T>& getHits() const
    {
      return static_cast<const vector<T>&>(*this); 
    }
   
    template<class T>
    vector<T>& getHits()  
    {
      return static_cast<const vector<T>&>(*this); 
    }


   
    template<class T>
    class const_iterator :
      public vector<T>::const_iterator
    {
    public:
      
      const_iterator() :
        vector<T>::const_iterator()
      {}
      
      const_iterator(const typename vector<T>::const_iterator& i) :
        vector<T>::const_iterator(i)
      {}
    };


    template<class T>
    class const_reverse_iterator :
      public vector<T>::const_reverse_iterator
    {
    public:
      const_reverse_iterator() :
        vector<T>::const_reverse_iterator()
      {}
      const_reverse_iterator(const typename vector<T>::const_reverse_iterator& i) :
        vector<T>::const_reverse_iterator(i)
      {}
    };

   
    template<class T>
    const_iterator<T> begin() const
    {
      return static_cast<const vector<T>&>(*this).begin();
    }
   
    template<class T>
    const_iterator<T> end() const
    {
      return static_cast<const vector<T>&>(*this).end();
    }

    template<class T>
    const_reverse_iterator<T> rbegin() const
    {
      return static_cast<const vector<T>&>(*this).rbegin();
    }

    template<class T>
    const_reverse_iterator<T> rend() const
    {
      return static_cast<const vector<T>&>(*this).rend();
    }

    template<class T>
    bool empty() const
    {
      return static_cast<const vector<T>&>(*this).empty();
    }

    template<class T>
    std::size_t size() const
    {
      return static_cast<const vector<T>&>(*this).size();
    }


   
    virtual int getSize() const 
    {
      return ( JDAQPreamble::sizeOf()                                                                                 +  
	       JDAQHeader::sizeOf()                                                                                   +
	       sizeof(int) + JDAQKeyHit::sizeOf()*static_cast<const vector<JDAQKeyHit>&>(*this).size()                +    
	       sizeof(int) + JDAQTriggeredHit::sizeOf()*static_cast<const vector<JDAQTriggeredHit>&>(*this).size()    +    
	       KM3OnlineTrack::sizeOf()                                                                               +
	       KM3OnlineMultiVariables::sizeOf()                                                                      );
    }

    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineEvent& event)
    {
      in >> static_cast<JDAQPreamble&>(event);
      in >> static_cast<JDAQHeader&>(event); 
      in >> static_cast<vector<JDAQTriggeredHit>&>(event); 
      in >> static_cast<vector<JDAQKeyHit>&>(event); 
      in >> event.track_;
      in >> event.multiVariables_;
                  
      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineEvent& event)
    {
      out << static_cast<const JDAQPreamble&>(event);
      out << static_cast<const JDAQHeader&>(event); 
      out << static_cast<const vector<JDAQTriggeredHit>&>(event); 
      out << static_cast<const vector<JDAQKeyHit>&>(event); 
      out << event.track_;
      out << event.multiVariables_;      

      return out;
    }


    friend inline std::ostream& operator<<(std::ostream& out, const KM3OnlineEvent& event)
    {
      using std::endl;

      out << "******************* " << endl;
      out << "   KM3OnlineEvent:" << endl;
      out << "******************* " << endl;
      
      out <<"Detector ID       " << event.getDetectorID()                                      << endl;
      out <<"Run Number        " << event.getRunNumber()                                       << endl;
      out <<"N-Snapshot  hits  " << static_cast<const vector<JDAQKeyHit>&>(event).size()       << endl;
      out <<"N-Triggered  hits " << static_cast<const vector<JDAQTriggeredHit>&>(event).size() << endl;
      out <<event.track_                                                                       << endl;
      out <<event.multiVariables_                                                              << endl;
                  
      return out;
    }

    ClassDef(KM3OnlineEvent,1);
  };

}

#endif

