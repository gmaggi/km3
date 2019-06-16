#ifndef KM3_H5_REGISTERED_DATATYPE_INCLUDED
#define KM3_H5_REGISTERED_DATATYPE_INCLUDED

#include <limits>

namespace H5Converter
{
  /**
   *  These are wrapper objects for Plain Old Data to enable HOFFSET for CompType
  */
  struct ParticleWrapper
  {
    double pos_x   = std::numeric_limits<double>::quiet_NaN();
    double pos_y   = std::numeric_limits<double>::quiet_NaN();
    double pos_z   = std::numeric_limits<double>::quiet_NaN();
    double dir_x   = std::numeric_limits<double>::quiet_NaN();
    double dir_y   = std::numeric_limits<double>::quiet_NaN(); 
    double dir_z   = std::numeric_limits<double>::quiet_NaN();
    double energy  = std::numeric_limits<double>::quiet_NaN(); 
    double quality = std::numeric_limits<double>::quiet_NaN();  
    double time    = std::numeric_limits<double>::quiet_NaN(); 

    int is_in_event;

    //For Reco-fit only:
    int ndf, last_history, status;

    //For MC only    
    int mc_type;
  };


  template <typename T>
  struct PODWrapper {
    T VALUE;
    int is_in_event;
  }; 


  struct GeneratedInfo
  {
    double center_x, center_y, center_z;

    double can_zmin, can_zmax, can_radius;
    
    double genvol_zmin, genvol_zmax, genvol_radius, genvol_volume, genvol_numberOfEvents;
   
    double livetime_n_seconds; 
    double livetime_error_seconds;
    
    double spectrum_alpha;
  };

}

#endif


  
