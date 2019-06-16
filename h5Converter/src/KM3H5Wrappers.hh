#ifndef KM3_H5_WRAPPERS_INCLUDED
#define KM3_H5_WRAPPERS_INCLUDED

#include "JFit/JEvt.hh"

#include "evt/Evt.hh"

#include "KM3H5RegisteredDataType.hh"

namespace H5Converter
{
  namespace WrapperForH5     
  {
    inline ParticleWrapper
    PrePopulateTable(const JFIT::JFit &track, 
			    const bool IsInEvent=true) 
    {
      ParticleWrapper jppwrapper;
      if ( not IsInEvent ) return jppwrapper;

      jppwrapper.pos_x  = track.getX();
      jppwrapper.pos_y  = track.getY();
      jppwrapper.pos_z  = track.getZ();
      
      jppwrapper.dir_x = track.getDX();
      jppwrapper.dir_y = track.getDY();
      jppwrapper.dir_z = track.getDZ();
      
      jppwrapper.energy  = track.getE();
      jppwrapper.ndf     = track.getNDF();
      jppwrapper.quality = track.getQ();
      jppwrapper.time    = track.getT();
      jppwrapper.status  = track.getStatus();
      
      jppwrapper.last_history = -1;
      const JFIT::JHistory &history= track.getHistory();
      if (not history.empty()) {jppwrapper.last_history = history.back().type;} 

      //Since this is a reco fit, the number below has no meaning, which normally represents PDG code      
      jppwrapper.mc_type = -1;
     
      jppwrapper.is_in_event = IsInEvent;
     
      return jppwrapper;
    }


    inline ParticleWrapper
    PrePopulateTable(const Trk &track, 
		     const bool IsInEvent=true) 
    {
      ParticleWrapper jppwrapper;
      if ( not IsInEvent ) return jppwrapper;
      
      jppwrapper.pos_x  = track.pos.x;
      jppwrapper.pos_y  = track.pos.y;
      jppwrapper.pos_z  = track.pos.z;
      
      jppwrapper.dir_x = track.dir.x;
      jppwrapper.dir_y = track.dir.y;
      jppwrapper.dir_z = track.dir.z;
      
      jppwrapper.energy  = track.E;
      jppwrapper.time    = track.t;
      jppwrapper.mc_type = track.type;

      jppwrapper.is_in_event = IsInEvent;

      //Since this is an MC Particle, numbers below have no meaning
      jppwrapper.status       = -1;
      jppwrapper.last_history = -1;
      jppwrapper.ndf          = -1;
      jppwrapper.quality      = std::numeric_limits<double>::quiet_NaN();  

      return jppwrapper;
    }

   
    template<typename T> 
    inline PODWrapper<T> 
    PrePopulateTable(const T value,
		     const bool IsInEvent=true) 
    {
      PODWrapper<T> podWrapper;

      podWrapper.VALUE       = value;
      podWrapper.is_in_event = IsInEvent;

      return podWrapper;
    } 
  }
  
}

#endif


  
