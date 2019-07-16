#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "KM3Online/KM3OnlineTags.hh"

#include "JSupport/JMeta.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHostObjectIterator.hh"
#include "JNet/JControlHostObjectOutput.hh"
#include "JLang/JSinglePointer.hh"

#include "KM3Online/KM3OnlineScoredEvent.hh"
#include "KM3Online/KM3OnlineEvent.hh"
#include "KM3Online/KM3OnlineTrack.hh"


int main(int argc, char **argv)
{
  using KM3ONLINE::KM3OnlineScoredEvent;
  using JNET::JControlHostObjectOutput;
  using std::endl;
  using std::cout;

  JPARSER::JParser<> zap("KM3OnlineBuildKM3OnlineScoredEvent");
  
  std::string HOSTNAME;
  std::string LISTENER;
  int debug;

  zap['H'] = make_field(HOSTNAME)  ;
  zap['L'] = make_field(LISTENER)  ;
  zap['d'] = make_field(debug)     = 1;

  zap.read(argc, argv);

  typedef KM3ONLINE::KM3OnlineScoredEvent data_type;

  JLANG::JSinglePointer<JControlHostObjectOutput<data_type> > listener;
 
  const unsigned timeOut(60);
  for (unsigned n_trials = 1; n_trials <= timeOut; ++n_trials){

    DEBUG("Trying to connect to LISTENER, trial number: " <<n_trials<<endl);

    try {
      listener.reset(new JControlHostObjectOutput<data_type>( LISTENER ) );
      DEBUG("Succeeded Listener conection"<<endl);
      break;
    }
    catch(const std::exception& error) {
      sleep(1);
      DEBUG("Failed Listener conection"<<endl);
    }
  }


  try{
    JNET::JControlHostObjectIterator<KM3ONLINE::KM3OnlineEvent> data( HOSTNAME );

    
    while ( data.hasNext() ){
     
      const KM3ONLINE::KM3OnlineEvent *in = data.next();
   
      const double any_score=0.8;

      data_type scoredEvent(*in,any_score);

      cout<<scoredEvent<<endl;

      try { listener->put(scoredEvent); }
      catch(const std::exception& error)  { ERROR(error.what()<<" at KM3OnlineBuildKM3OnlineScoredEvent" << endl); }      
    }      
  }
  catch(const JLANG::JSocketException& error) {
    ERROR(error<< " at KM3OnlineScoredGet " << std::endl);
  }
      
  return 0;
}

