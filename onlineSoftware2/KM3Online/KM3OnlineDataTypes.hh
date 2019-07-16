#ifndef __KM3ONLINEDATATYPES__
#define __KM3ONLINEDATATYPES__

#include "JLang/JType.hh"
#include "JDAQ/JDAQDataTypes.hh"

namespace KM3ONLINE { 
  struct KM3OnlineEvent; 
  struct KM3OnlineScoredEvent; 
}

namespace KM3NETDAQ {
  inline int getDataType(JDAQType<KM3ONLINE::KM3OnlineEvent>)       { return 100001; }
  inline int getDataType(JDAQType<KM3ONLINE::KM3OnlineScoredEvent>) { return 100003; }
}

#endif
