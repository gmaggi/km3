#ifndef KM3ONLINEFILEWRITER_INCLUDED
#define KM3ONLINEFILEWRITER_INCLUDED

#include <string>
#include <vector>
#include <cstdlib>

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>

#include "evt/Head.hh"
#include "evt/Evt.hh"

#include "JFit/JFitApplications.hh"
#include "JAAnet/JHead.hh"
#include "JSupport/JMeta.hh"
#include "JAAnet/JHeadToolkit.hh"
#include "JSupport/JFileRecorder.hh"
#include "JSupport/JSupport.hh"

namespace KM3ONLINE
{

  class KM3OnlineFileWriter
  {
  private:
    std::string directory_;
    std::string detector_;
    std::string fullPath_;
    
  public:
    
    KM3OnlineFileWriter(const std::string directory,
			const std::string detector):
      directory_(directory),
      detector_(detector)
    {}
    
    ~KM3OnlineFileWriter()
    {}
    
    
    static void make_Trk(const KM3ONLINE::KM3OnlineTrack &in,
			 std::vector<Trk> &outs)
    {
    Trk out;
    
    out.id         = outs.size()+1;
    out.pos        = Vec(in.getX(),  in.getY(),  in.getZ());
    out.dir        = Vec(in.getDX(), in.getDY(), in.getDZ());
    out.t          = in.getTime();
    out.E          = in.getEnergy();
    out.lik        = in.getQuality();
    out.rec_type   = in.getType();
    out.rec_stages = {1,2,3};
    
    outs.push_back(out);
    }
    

    std::string makeDirectory(const int detectorID, 
			      const int runNumber)
    {
      namespace bf = boost::filesystem;
      using std::string;
           
      const std::size_t max_str_date=2;
      const std::size_t max_str_run=8;
      const std::size_t max_str_detId=8;
            
      string detID = std::to_string(detectorID);
      string run   = std::to_string(runNumber);

      while ( detID.length() < max_str_detId  ) detID.insert(0,"0");
      while ( run.length()   < max_str_run    ) run.insert(0,"0");


      boost::posix_time::ptime localTime = boost::posix_time::second_clock::local_time();
      
      const string year = std::to_string( static_cast<int>( localTime.date().year() ) );

      string month = std::to_string( static_cast<int>( localTime.date().month() ) );
      if ( month.length() < max_str_date ) month.insert(0,"0");   
      
      string day   = std::to_string( localTime.date().day() );  
      if ( day.length()   < max_str_date ) day.insert(0,"0");   

      
      const string detectorPath = directory_+"/"+year+"/";    
      if ( not bf::exists(detectorPath) ) { bf::create_directory(detectorPath); }
      
      const string monthPath    = detectorPath+month+"/";
      if ( not bf::exists(monthPath) )    { bf::create_directory(monthPath);     } 

      const string dayPath      = monthPath+day+"/";
      if ( not bf::exists(dayPath) )    { bf::create_directory(dayPath);     } 
            
      
      const string fileName = detector_+"KM3Online"+"."+detID+"_"+run+".root";  

      fullPath_ = dayPath+fileName;

      return fullPath_;
    }
    
    void writeFile(const std::vector<Evt> &events, 
		   JSUPPORT::JMeta &meta) const
    { 
      string ONLINE_RECO_VERSION("");
      if ( const char* olVersion = std::getenv("ONLINE_RECO_VERSION") ) { ONLINE_RECO_VERSION = string(olVersion); }
   
      JSUPPORT::JFileRecorder<JTYPELIST<Head, Evt, JMeta>::typelist> outputFile(fullPath_.c_str());
      outputFile.open();

      meta["GIT_ONLINE_RECO"] = ONLINE_RECO_VERSION;
      meta["OUT_FILE"]        = fullPath_;
      outputFile.put(meta);

      for (std::vector<Evt>::const_iterator event = events.begin();
	   event != events.end(); ++event){
	
	outputFile.put(*event);
      }
      outputFile.close();
    }
    
  };
}

#endif


