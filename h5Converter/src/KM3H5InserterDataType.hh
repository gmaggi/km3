#ifndef KM3_H5_INSERTER_DATATYPE
#define KM3_H5_INSERTER_DATATYPE

#include <vector>
#include <memory>

#include "H5Cpp.h"

#include "KM3H5RegisteredDataType.hh"

class KM3H5Converter;//forward declaration

namespace H5Converter
{
   /**
   * class with private members only to keep this data well encapsulated/hidden
  */
  class KM3H5InserterDataType
  {
    friend class KM3H5Converter;

    static std::shared_ptr<H5::CompType> 
    InsertMemberInH5CompType(const std::vector<ParticleWrapper> &) 
    {
      std::shared_ptr<H5::CompType> comptype= 
	std::make_shared<H5::CompType>(sizeof(ParticleWrapper));
      	
      comptype->insertMember("pos_x",  HOFFSET(ParticleWrapper,pos_x),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("pos_y",  HOFFSET(ParticleWrapper,pos_y),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("pos_z",  HOFFSET(ParticleWrapper,pos_z),  H5::PredType::NATIVE_DOUBLE);
      
      comptype->insertMember("dir_x", HOFFSET(ParticleWrapper,dir_x), H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("dir_y", HOFFSET(ParticleWrapper,dir_y), H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("dir_z", HOFFSET(ParticleWrapper,dir_z), H5::PredType::NATIVE_DOUBLE);
      
      comptype->insertMember("energy",  HOFFSET(ParticleWrapper,energy), H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("ndf",     HOFFSET(ParticleWrapper,ndf), H5::PredType::NATIVE_INT);
      comptype->insertMember("quality", HOFFSET(ParticleWrapper,quality), H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("time",    HOFFSET(ParticleWrapper,time), H5::PredType::NATIVE_DOUBLE);
      
      comptype->insertMember("status",  HOFFSET(ParticleWrapper,status), H5::PredType::NATIVE_INT);
      comptype->insertMember("mc_type", HOFFSET(ParticleWrapper,mc_type), H5::PredType::NATIVE_INT);

      comptype->insertMember("is_in_event", HOFFSET(ParticleWrapper,is_in_event), H5::PredType::NATIVE_HBOOL);
      
      return comptype;
    }

    template <typename T>
    static std::shared_ptr<H5::CompType> 
    InsertMemberInH5CompType(const std::vector<PODWrapper<T> > &) 
    {
      std::shared_ptr<H5::CompType> comptype= 
	std::make_shared<H5::CompType>(sizeof(PODWrapper<T>));
      
      const std::type_info &typeinfo= typeid(PODWrapper<T>);

      if ( typeinfo == typeid(PODWrapper<unsigned int>) ){
	comptype->insertMember("value", HOFFSET(PODWrapper<unsigned int>,VALUE), H5::PredType::NATIVE_UINT32);
	comptype->insertMember("is_in_event", HOFFSET(PODWrapper<unsigned int>,is_in_event), H5::PredType::NATIVE_HBOOL);
      }
      else if ( typeinfo == typeid(PODWrapper<int>) ){
	comptype->insertMember("value", HOFFSET(PODWrapper<int>,VALUE),H5::PredType::NATIVE_INT); 
	comptype->insertMember("is_in_event", HOFFSET(PODWrapper<int>,is_in_event), H5::PredType::NATIVE_HBOOL);
      }
      else if ( typeinfo == typeid(PODWrapper<double>) ){
	comptype->insertMember("value", HOFFSET(PODWrapper<double>,VALUE), H5::PredType::NATIVE_DOUBLE);
	comptype->insertMember("is_in_event", HOFFSET(PODWrapper<double>,is_in_event), H5::PredType::NATIVE_HBOOL);
      }
     
      return comptype;
    }
    
    static std::shared_ptr<H5::CompType> 
    InsertGeneratedInfoInH5CompType()
    {
      std::shared_ptr<H5::CompType> comptype= 
	std::make_shared<H5::CompType>(sizeof(GeneratedInfo));

      comptype->insertMember("center_x",  HOFFSET(GeneratedInfo,center_x),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("center_y",  HOFFSET(GeneratedInfo,center_y),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("center_z",  HOFFSET(GeneratedInfo,center_z),  H5::PredType::NATIVE_DOUBLE);

      comptype->insertMember("can_zmin",  HOFFSET(GeneratedInfo,can_zmin),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("can_zmax",  HOFFSET(GeneratedInfo,can_zmax),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("can_radius",HOFFSET(GeneratedInfo,can_radius),H5::PredType::NATIVE_DOUBLE);

      comptype->insertMember("genvol_zmin",  HOFFSET(GeneratedInfo,genvol_zmin),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("genvol_zmax",  HOFFSET(GeneratedInfo,genvol_zmax),  H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("genvol_radius",HOFFSET(GeneratedInfo,genvol_radius),H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("genvol_volume",HOFFSET(GeneratedInfo,genvol_volume),H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("genvol_numberOfEvents", HOFFSET(GeneratedInfo,genvol_numberOfEvents),
			     H5::PredType::NATIVE_DOUBLE);
      
      comptype->insertMember("livetime_n_seconds",     HOFFSET(GeneratedInfo,livetime_n_seconds),  
			     H5::PredType::NATIVE_DOUBLE);
      comptype->insertMember("livetime_error_seconds", HOFFSET(GeneratedInfo,livetime_error_seconds),  
			     H5::PredType::NATIVE_DOUBLE);

      comptype->insertMember("spectrum_alpha",HOFFSET(GeneratedInfo,spectrum_alpha),H5::PredType::NATIVE_DOUBLE);

      return comptype;
    }

  };
}

#endif


  
