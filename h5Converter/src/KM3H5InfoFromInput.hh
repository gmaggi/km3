#ifndef KM3_H5_INFO_FROM_INPUT_INCLUDED
#define KM3_H5_INFO_FROM_INPUT_INCLUDED

#include <algorithm>
#include <limits>
#include <vector>
#include <string>
#include <utility>

#include "JLang/JLangToolkit.hh"
#include "JLang/JObjectMultiplexer.hh"
#include "JSupport/JMultipleFileScanner.hh"
#include "JSupport/JMeta.hh"
#include "JSupport/JMonteCarloFileSupportkit.hh"
#include "JSupport/JSupport.hh"
#include "JFit/JEvt.hh"
#include "JAAnet/JHeadToolkit.hh"
#include "JAAnet/JAAnetToolkit.hh"

#include "evt/Evt.hh"

#include "KM3H5RegisteredDataType.hh"


namespace H5Converter
{
  class KM3H5InfoFromInput     
  {
    friend class KM3H5Converter;
    
    /**
     *  The method below stores only some generated information that is common in the full data set.
     *  What info would not be common?: start_run, seed, physics, physics_1 
    */
    static GeneratedInfo
    PreputateGeneratedInfoTable(const std::string fileName)
    {
      GeneratedInfo genInfo;

      JSUPPORT::JMultipleFileScanner<JAAnetTypes_t> inputFile(fileName);
      const JAANET::JHead jHead( JSUPPORT::getHeader(inputFile) );

      const JGEOMETRY3D::JVector3D &center 
	= JAANET::getPosition( jHead.coord_origin );
      genInfo.center_x = center.getX();
      genInfo.center_y = center.getY();
      genInfo.center_z = center.getZ();

      const JAANET::can &can = jHead.can; 
      genInfo.can_zmin   = can.zmin;
      genInfo.can_zmax   = can.zmax;
      genInfo.can_radius = can.r;

      const JAANET::genvol &genVol  = jHead.genvol;
      genInfo.genvol_zmin           = genVol.zmin;  
      genInfo.genvol_zmax           = genVol.zmax;
      genInfo.genvol_radius         = genVol.r;
      genInfo.genvol_volume         = genVol.volume;
      genInfo.genvol_numberOfEvents = genVol.numberOfEvents;
  
      const JAANET::livetime &liveTime = jHead.livetime;
      genInfo.livetime_n_seconds       = liveTime.numberOfSeconds;
      genInfo.livetime_error_seconds   = liveTime.errorOfSeconds;

      const JAANET::spectrum &spec = jHead.spectrum;
      genInfo.spectrum_alpha       = spec.alpha;

      return genInfo;
    }

    static std::vector<std::pair<std::string,std::string> >
    PrepopulateGeneratorInfoAttributes(const std::string fileName)
    {
      using std::string;

      std::vector<std::pair<string,string> > stringPairs;

      JSUPPORT::JMultipleFileScanner<JAAnetTypes_t> inputFile(fileName);
      const JAANET::JHead jHead( JSUPPORT::getHeader(inputFile) );

      const std::vector<JAANET::physics> &physicsVec = jHead.physics;
      for (std::vector<JAANET::physics>::const_iterator generator = physicsVec.begin(); 
	   generator != physicsVec.end(); ++generator){

	const std::pair<string,string> &pair = 
	  std::make_pair(generator->program,generator->version);

	stringPairs.push_back(pair);
      }

      const JAANET::XSecFile &xSecFile = jHead.XSecFile;
      const string xSecBuffer = xSecFile.buffer;

      if ( not xSecBuffer.empty() ) {
	const std::pair<string,string> xsec = std::make_pair("XSecFile",xSecBuffer);
	stringPairs.push_back(xsec);
      }
      else{
	std::cout<<"WARNING: Xsec File location has not been specified in input file"<<std::endl;
      }

      return stringPairs;
    }

    static std::vector<std::pair<std::string,std::string> >
    GetMetaFromInputFile(const std::string inputFileName)
    {
      std::vector<std::pair<std::string,std::string> > outMetas;

      JSUPPORT::JMultipleFileScanner<JMetaTypes_t> inputFile(inputFileName);
      JLANG::JObjectMultiplexer<JMetaTypes_t> metas(inputFile);
      
      while ( metas.hasNext() ) {
	
	JSUPPORT::JMeta *meta = metas.next();
	
	std::string application;
	std::string command;
	for (JMeta::const_iterator i = meta->begin(); i != meta->end(); ++i) {
	
	  if ( JLANG::double_quote(i->first).find("application") != std::string::npos ){
	    application = JLANG::double_quote(i->second);
	    application.erase( std::remove(application.begin(), application.end(), '"'), application.end() );
	  }

	  if ( JLANG::double_quote(i->first).find("command") != std::string::npos ){
	    command = JLANG::double_quote(i->second);
	    command.erase( std::remove(command.begin(), command.end(), '"'), command.end() );
	  }
	
	}
	outMetas.push_back( std::make_pair(application,command)  );
      }
      
      return outMetas;
    }

  };

  
}

#endif


  
