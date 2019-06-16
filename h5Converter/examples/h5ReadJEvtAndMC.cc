#include <iostream>
#include <cmath>
#include <memory>

#include "KM3H5Converter.hh"

#include "JFit/JEvt.hh"

#include "JSupport/JTriggeredFileScanner.hh"
#include "Jeep/JParser.hh"
#include "JAAnet/JAAnetToolkit.hh"

int main(int argc, char **argv)
{
  using std::cout;
  using std::endl;
  using std::make_pair;
  using std::pair;
  using std::string;
  
  using JSUPPORT::JTriggeredFileScanner;
  using JFIT::JFit;

  H5::Exception::dontPrint();

  string OUTPUTFILENAME;
  JTriggeredFileScanner<JFIT::JEvt> INPUTFILE;
  JSUPPORT::JLimit_t &NUMBERofEVENTS = INPUTFILE.getLimit();
  int debug;

  JParser<> zap("Script to convert Online info into H5 files");
  zap['f'] = make_field(INPUTFILE);
  zap['o'] = make_field(OUTPUTFILENAME) ="out.h5";
  zap['n'] = make_field(NUMBERofEVENTS) = JSUPPORT::JLimit::max();    
  zap['d'] = make_field(debug)          = 1;    

  if (zap.read(argc, argv) != 0) return 1;

  string inputFileName = INPUTFILE.getFilename();
  H5Converter::KM3H5Converter h5Converter(inputFileName,OUTPUTFILENAME);

  //Below, name of the groups(first str), and datasets(second str) as they will appear in the output file
  //Give any name that you want to the groups and Data Sets

  const pair<string,string> mc_muonTable      = make_pair("mc_Group","mc_muon");
  const pair<string,string> mc_w1Table        = make_pair("mc_Group","mc_w1");
  const pair<string,string> myprefitTable     = make_pair("myRecoGroup","myprefit");
    
  //Table for generated info, this includes can, genvol, live time, spectrum, etc. 

  h5Converter.WriteTableForGeneratedInfo(make_pair("MCGroup","GeneratedInfo"));

  //Push Back variables into the dataset
  int c=1;
  while (INPUTFILE.hasNext()) {
    STATUS("event: " << setw(10) << INPUTFILE.getCounter() << '\r'); DEBUG(endl);

    JTriggeredFileScanner<JFIT::JEvt>::multi_pointer_type ps = INPUTFILE.next();
    const JFIT::JEvt *jEvt= ps;
    const Evt *mc_evt = ps;

    if ( JAANET::has_neutrino(*mc_evt) ){
      const Trk &mc_neutrino   = JAANET::get_neutrino(*mc_evt);
      h5Converter.PushBack(mc_neutrino, "mc_Group","mc_neutrino");
      
      const double mc_w1      = mc_evt->w.at(1);
      h5Converter.PushBack(mc_w1          , mc_w1Table);
    }
    
    if ( JAANET::has_muon(*mc_evt) ){
      const Trk &mc_muon = JAANET::get_muon(*mc_evt);
      h5Converter.PushBack(mc_muon   , mc_muonTable);
    }

    if ( not jEvt->empty() ){
      h5Converter.PushBack(jEvt->front(), myprefitTable);
    }

    if (c >= 10 and c<=25) h5Converter.PushBack(c*(-1),"anygroup","whatevertable");
        
    h5Converter.CheckNcallsForPushBack();
    c+=1;
  }
  STATUS(endl);
  
  h5Converter.WriteTables();
  h5Converter.CloseFile();

  return 0;
}

