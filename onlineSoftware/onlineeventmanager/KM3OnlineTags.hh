#ifndef KM3ONLINETAGS_INCLUDE
#define KM3ONLINETAGS_INCLUDE

#include <string>

#include "JNet/JTag.hh"
#include "JLang/JType.hh"

namespace KM3ONLINE { 
  static const JNET::JTag IO_KM3ONLINE        = JNET::JTag("IO_OLINE"); 
  static const JNET::JTag IO_KM3ONLINEGRAPHIC = JNET::JTag("IO_RECO"); 
} 

namespace KM3ONLINE { 
  class KM3OnlineEvent;
  class KM3OnlineGraphic; 

}

inline JNET::JTag getTag(JLANG::JType<KM3ONLINE::KM3OnlineEvent>)   { return KM3ONLINE::IO_KM3ONLINE; }
inline JNET::JTag getTag(JLANG::JType<KM3ONLINE::KM3OnlineGraphic>) { return KM3ONLINE::IO_KM3ONLINEGRAPHIC; }


#endif
