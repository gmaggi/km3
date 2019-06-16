#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "JIO/JByteArrayIO.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHost.hh"

int main(int argc, const char *argv[])
{
  using std::cout;
  using std::endl;
  
  std::string HOST;

  try {
    JPARSER::JParser<> zap("Read a single online event just by subcribing to the tag IO_OLINE");
    zap['H'] = make_field(HOST) ;
    
    zap(argc, argv);
  }
  catch(const std::exception &error) {
    FATAL(error.what() << endl);
  }


  JNET::JControlHost::Throw(true); 
  
  try {
    JNET::JControlHost in(HOST);
    JNET::JSubscription subscription(JNET::SUBSCRIBE_ANY,JNET::JTag("IO_OLINE"));

    in.Subscribe(subscription);
    in.SendMeAlways();

    JNET::JPrefix      prefix;
    std::vector<char> buffer;

    in.WaitHead(prefix);
    buffer.resize(prefix.getSize());
    in.GetFullData(buffer.data(), buffer.size());

    cout<<prefix.getTag() <<" size "<<buffer.size()<<endl;

    buffer.erase(buffer.begin(),buffer.begin()+sizeof(int)*2); //<- remove 8 bytes which correspond to the size of the online-preamble

    JIO::JByteArrayReader data(buffer.data(), buffer.size());

    int detId;
    int run;
    int frameIndex;
    unsigned long long int triggerCounter;
    unsigned int utc;
   
    //for shower
    double sx, sy, sz, sdx, sdy, sdz, se, sq, st;
    int stype, sstatus, sndf;

    //for track
    double tx, ty, tz, tdx, tdy, tdz, te, tq, tt;
    int ttype, tstatus, tndf;
    
    data >> detId >> run >> frameIndex >> triggerCounter >> utc  >> sx>> sy>> sz>> sdx>> sdy>> sdz>> se>> sq>> st>> stype>> sstatus>> sndf   >> tx>> ty>> tz>> tdx>> tdy>> tdz>> te>> tq>> tt>> ttype>> tstatus>> tndf;

    cout<<"DetID "<<detId<<" RUN "<<run<<" frameIndex "<<frameIndex<<" triggerCounter "<<triggerCounter<<" utc "<<utc<<endl;

    cout<<"Track X: "<<tx<<" Y: "<<ty<<" Z: "<<tz\
	<<" DX: "<<tdx<<" DY: "<<tdy<<" DZ: "<<tdz\
	<<" Energy: "<<te<<" Quality: "<<tq<<" Time: "<<tt\
	<<" Type: "<<ttype<<" Status: "<<tstatus<<" NDF "<<tndf<<endl;
  }    
  catch(const JNET::JControlHostException& error) {
    ERROR(error << endl);
  }
}


