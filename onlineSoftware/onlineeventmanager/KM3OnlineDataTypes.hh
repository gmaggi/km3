#ifndef __KM3ONLINEDATATYPES__
#define __KM3ONLINEDATATYPES__

#include "JLang/JType.hh"
#include "JDAQ/JDAQDataTypes.hh"

namespace KM3ONLINE { 
  struct KM3OnlineEvent; 
  struct KM3OnlineGraphic; 
}

namespace KM3NETDAQ {
  inline int getDataType(JDAQType<KM3ONLINE::KM3OnlineEvent>)   { return 100001; }
  inline int getDataType(JDAQType<KM3ONLINE::KM3OnlineGraphic>) { return 100002; }
}

#endif
