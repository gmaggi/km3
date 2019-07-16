#ifndef H5_CONVETER_ONLINE_INCLUDED
#define H5_CONVETER_ONLINE_INCLUDED

#include <string>

#include "JFit/JFitApplications.hh"
#include "JFit/JEvt.hh"

#include "src/KM3H5Converter.hh"
#include "onlineeventmanager/KM3OnlineEvent.hh"

namespace KM3ONLINE
{
  /**
    Make a JFit to be handled by the h5 convertor
   */

  JFIT::JFit make_JFit(const KM3OnlineTrack &track)
  {
    const JFIT::JHistory history( static_cast<JFIT::JFitApplication_t>(track.getType()) );

    return JFIT::JFit(history,
		      track.getX(),track.getY(),track.getZ(),
		      track.getDX(),track.getDY(),track.getDZ(),
		      track.getTime(),track.getQuality(),track.getNDF(),
		      track.getEnergy(),track.getStatus());
  }

  struct KM3H5OnlineConverter: 
    H5Converter::KM3H5Converter
  {
    KM3H5OnlineConverter(const std::string outfile):
      H5Converter::KM3H5Converter("",outfile)
    {}
        
    virtual ~KM3H5OnlineConverter()
    {}

    void PushBackAll(const KM3ONLINE::KM3OnlineEvent &event)
    {
      H5::Exception::dontPrint();

      this->PushBack( event.GetDetectorID()              ,"event","detector_id" );
      this->PushBack( event.GetRunNumber()               ,"event","run_number" );
      this->PushBack( event.GetFrameIndex()              ,"event","frame_index" );
      this->PushBack( event.GetUTCTimeSliceStartSec()    ,"event","timeUTC_sliceStartSec" );

      this->PushBack( make_JFit( event.GetRecoShower() ) ,"reco" ,"shower");
      this->PushBack( make_JFit( event.GetRecoTrack()  ) ,"reco" ,"track");
    }
    
  };
}
#endif
