#ifndef KM3ONLINETAGS_INCLUDE
#define KM3ONLINETAGS_INCLUDE

#include <string>

#include "JNet/JTag.hh"
#include "JLang/JType.hh"

namespace KM3ONLINE { 
  static const JNET::JTag IO_KM3ONLINE        = JNET::JTag("IO_RECO"); 
  static const JNET::JTag IO_KM3ONLINESCORED  = JNET::JTag("IO_SCORE"); 
} 

namespace KM3ONLINE { 
  class KM3OnlineEvent;
  class KM3OnlineScoredEvent; 
}

inline JNET::JTag getTag(JLANG::JType<KM3ONLINE::KM3OnlineEvent>)       { return KM3ONLINE::IO_KM3ONLINE;        }
inline JNET::JTag getTag(JLANG::JType<KM3ONLINE::KM3OnlineScoredEvent>) { return KM3ONLINE::IO_KM3ONLINESCORED;  }


#endif
