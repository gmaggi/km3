#ifndef __KM3ONLINEGRAPHIC__
#define __KM3ONLINEGRAPHIC__

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


namespace KM3ONLINE
{
  using KM3NETDAQ::JDAQEvent;
  using KM3NETDAQ::JDAQPreamble;
  using KM3NETDAQ::JDAQHeader;
  using KM3NETDAQ::JDAQKeyHit;
  using KM3NETDAQ::JDAQTriggeredHit;
    
  class KM3OnlineGraphic:
    public JDAQPreamble,
    public JDAQHeader,
    public std::vector<JDAQTriggeredHit>,
    public std::vector<JDAQKeyHit>
  {
  private:

    KM3OnlineTrack track_;
    
  public:    
      
    KM3OnlineGraphic() :
      JDAQPreamble(KM3NETDAQ::JDAQType<KM3OnlineGraphic>()),
      JDAQHeader(),
      std::vector<JDAQTriggeredHit>(),
      std::vector<JDAQKeyHit>(),
      track_()
    {} 
    
    KM3OnlineGraphic(const JDAQHeader &header, 
		     const std::vector<KM3NETDAQ::JDAQTriggeredHit> &trigHits, 
		     const std::vector<KM3NETDAQ::JDAQKeyHit> &keyHits, 
		     const KM3OnlineTrack &track):
      JDAQPreamble(KM3NETDAQ::JDAQType<KM3OnlineGraphic>()),
      JDAQHeader(header),
      std::vector<JDAQTriggeredHit>(trigHits),
      std::vector<JDAQKeyHit>(keyHits),
      track_(track)
    {}
         
    ~KM3OnlineGraphic()
    {}

    KM3OnlineTrack getTrack() const
    {
      return track_;
    }

    JDAQHeader getDAQHeader() const
    {
      return static_cast<const JDAQHeader&>(*this);
    } 

    template<class T>
    const std::vector<T>& getHits() const
    {
      return static_cast<const std::vector<T>&>(*this); 
    }
   
    template<class T>
    std::vector<T>& getHits()  
    {
      return static_cast<const std::vector<T>&>(*this); 
    }


   
    template<class T>
    class const_iterator :
      public std::vector<T>::const_iterator
    {
    public:
      
      const_iterator() :
        std::vector<T>::const_iterator()
      {}
      
      const_iterator(const typename std::vector<T>::const_iterator& i) :
        std::vector<T>::const_iterator(i)
      {}
    };


    template<class T>
    class const_reverse_iterator :
      public std::vector<T>::const_reverse_iterator
    {
    public:
      const_reverse_iterator() :
        std::vector<T>::const_reverse_iterator()
      {}
      const_reverse_iterator(const typename std::vector<T>::const_reverse_iterator& i) :
        std::vector<T>::const_reverse_iterator(i)
      {}
    };

   
    template<class T>
    const_iterator<T> begin() const
    {
      return static_cast<const std::vector<T>&>(*this).begin();
    }
   
    template<class T>
    const_iterator<T> end() const
    {
      return static_cast<const std::vector<T>&>(*this).end();
    }

    template<class T>
    const_reverse_iterator<T> rbegin() const
    {
      return static_cast<const std::vector<T>&>(*this).rbegin();
    }

    template<class T>
    const_reverse_iterator<T> rend() const
    {
      return static_cast<const std::vector<T>&>(*this).rend();
    }

    template<class T>
    bool empty() const
    {
      return static_cast<const std::vector<T>&>(*this).empty();
    }

    template<class T>
    std::size_t size() const
    {
      return static_cast<const std::vector<T>&>(*this).size();
    }


   
    int getSize() const 
    {
      return ( JDAQPreamble::sizeOf()                                                                                  +  
	       JDAQHeader::sizeOf()                                                                                    +
	       sizeof(int) + JDAQKeyHit::sizeOf()*static_cast<const std::vector<JDAQKeyHit>&>(*this).size()            +    
	       sizeof(int) + JDAQTriggeredHit::sizeOf()*static_cast<const std::vector<JDAQTriggeredHit>&>(*this).size()+    
	       KM3OnlineTrack::sizeOf()                                                                                );
    }

    friend inline JIO::JReader& operator>>(JIO::JReader& in, KM3OnlineGraphic& graphic)
    {
      in >> static_cast<JDAQPreamble&>(graphic);
      in >> static_cast<JDAQHeader&>(graphic); 
      in >> static_cast<std::vector<JDAQTriggeredHit>&>(graphic); 
      in >> static_cast<std::vector<JDAQKeyHit>&>(graphic); 
      in >> graphic.track_;
                  
      return in;
    }

    friend inline JIO::JWriter& operator<<(JIO::JWriter& out, const KM3OnlineGraphic& graphic)
    {
      out << static_cast<const JDAQPreamble&>(graphic);
      out << static_cast<const JDAQHeader&>(graphic); 
      out << static_cast<const std::vector<JDAQTriggeredHit>&>(graphic); 
      out << static_cast<const std::vector<JDAQKeyHit>&>(graphic); 
      out << graphic.track_;
      
      return out;
    }


    friend inline std::ostream& operator<<(std::ostream& out, const KM3OnlineGraphic& graphic)
    {
      using std::endl;

      out << "******************* " << endl;
      out << "   KM3OnlineGraphic:" << endl;
      out << "******************* " << endl;
      
      out <<"Detector ID       " << graphic.getDetectorID()                                           << endl;
      out <<"Run Number        " << graphic.getRunNumber()                                            << endl;
      out <<"N-Snapshot  hits  " << static_cast<const std::vector<JDAQKeyHit>&>(graphic).size()       << endl;
      out <<"N-Triggered  hits " << static_cast<const std::vector<JDAQTriggeredHit>&>(graphic).size() << endl;
      out <<"Track             " << graphic.getTrack()                                                << endl;
                  
      return out;
    }

    ClassDef(KM3OnlineGraphic,1);
  };

}

#endif

