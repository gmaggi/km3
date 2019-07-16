#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "JIO/JByteArrayIO.hh"
#include "Jeep/JParser.hh"
#include "JNet/JControlHost.hh"

//see sizes in from https://wiki.km3net.de/index.php/Dataformats, and from JPP objects
int main(int argc, const char *argv[])
{
  using std::cout;
  using std::endl;

  std::string HOST;

  try {
    JPARSER::JParser<> zap("Read a single online event just by subcribing to the tag IO_RECO. This is independent of KM3Online objects");
    zap['H'] = make_field(HOST) ;
    
    zap(argc, argv);
  }
  catch(const std::exception &error) {
    FATAL(error.what() << endl);
  }


  JNET::JControlHost::Throw(true); 
  
  try {
    JNET::JControlHost in(HOST);
    JNET::JSubscription subscription(JNET::SUBSCRIBE_ANY,JNET::JTag("IO_RECO"));

    in.Subscribe(subscription);
    in.SendMeAlways();

    JNET::JPrefix     prefix;
    std::vector<char> buffer;

    in.WaitHead(prefix);
    buffer.resize(prefix.getSize());
    in.GetFullData(buffer.data(), buffer.size());

    cout<<prefix.getTag() <<" size "<<buffer.size()<<endl;

    //============== Remove JDAQPreamble and JDAQHeader

    const int preambleSize = sizeof(int)*2;
    const int headerSize   = sizeof(int)*3+sizeof(unsigned int)*2; //<-JDAQHeader/JDAQChronometer
          
    buffer.erase( buffer.begin(),
		  buffer.begin()+ (preambleSize+headerSize)  ); //<- remove JDAQPreamble and JDAQEventHeader
    
    //============= Get triggered hits (18 bytes) and then snapshot hits
   
    int n_hits;
    const std::vector<char> bufferNHits ( buffer.begin(), 
					  buffer.begin()+ sizeof(int) );
    JIO::JByteArrayReader dataNHits ( bufferNHits.data()  , bufferNHits.size()  );
    dataNHits >> n_hits;
    cout<<">>>> N-Triggered-HITS "<<n_hits<<endl;
   
    unsigned long long int  mask;
    int module;
    unsigned char pmt;
    unsigned int  tdc;
    unsigned char tot;
    const int sizeHits = ( sizeof(unsigned long long int)+
			   sizeof(int)+
			   sizeof(unsigned char)+
			   sizeof(unsigned int)+
			   sizeof(unsigned char) )*n_hits;

    const std::vector<char> bufferHits ( buffer.begin()+ sizeof(int), 
					 buffer.begin()+ sizeof(int) + sizeHits);
    JIO::JByteArrayReader dataHits ( bufferHits.data()  , bufferHits.size()  );

    for (int hit=0; hit < n_hits; ++hit){
      dataHits >> module >> pmt >> tdc >> tot >> mask;
      cout<<"module "<<module<<" PMT "<<static_cast<int>(pmt)<<" TDC "<<ntohl(tdc)<<" TOT "<<static_cast<int>(tot)<<endl;  
    }
  
    //snap shot hits

    int n_snapHits;
    const std::vector<char> bufferNsnapHits ( buffer.begin() + sizeof(int) + sizeHits , 
					      buffer.begin() + sizeof(int) + sizeHits + sizeof(int) );
    JIO::JByteArrayReader dataNsnapHits ( bufferNsnapHits.data()  , bufferNsnapHits.size()  );
    dataNsnapHits >> n_snapHits;
    cout<<">>>>> N-Snap shot-HITS "<<n_snapHits<<endl;

    const int sizeSnapHits = ( sizeof(int)+
			       sizeof(unsigned char)+
			       sizeof(unsigned int)+
			       sizeof(unsigned char) )*n_snapHits;

    const std::vector<char> bufferSnapHits ( buffer.begin() + sizeof(int) + sizeHits + sizeof(int), 
					     buffer.begin() + sizeof(int) + sizeHits + sizeof(int) + sizeSnapHits);
				       
    JIO::JByteArrayReader dataSnapHits ( bufferSnapHits.data()  , bufferSnapHits.size()  );

    for (int hit=0; hit < n_snapHits; ++hit){
      dataSnapHits >> module >> pmt >> tdc >> tot;
      cout<<"module "<<module<<" PMT "<<static_cast<int>(pmt)<<" TDC "<<ntohl(tdc)<<" TOT "<<static_cast<int>(tot)<<endl;  
    }


    //================ TRACK =================

    double tx, ty, tz, tdx, tdy, tdz, te, tq, tt;
    int ttype, tstatus, tndf;

    const std::vector<char> bufferTrack( buffer.begin()+ sizeof(int) + sizeHits + sizeof(int) + sizeSnapHits , 
					 buffer.end() ); 
    JIO::JByteArrayReader dataTrack( bufferTrack.data() , bufferTrack.size() );
            
    dataTrack >> tx>> ty>> tz>> tdx>> tdy>> tdz>> te>> tq>> tt>> ttype>> tstatus>> tndf;

    cout<<"Track X: "<<tx<<" Y: "<<ty<<" Z: "<<tz<<\
      " DX: "<<tdx<<" DY: "<<tdy<<" DZ: "<<tdz<<\
      " Energy: "<<te<<" Quality: "<<tq<<" Time: "<<tt<<\
      " Type: "<<ttype<<" Status: "<<tstatus<<" NDF "<<tndf<<endl;


  }    
  catch(const JNET::JControlHostException& error) {
    ERROR(error << endl);
  }
}


