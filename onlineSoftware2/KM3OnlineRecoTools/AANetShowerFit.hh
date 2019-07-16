#ifndef AANETSHOWERFIT_INCLUDE
#define AANETSHOWERFIT_INCLUDE

/**
   The author of these lines is Aart Heijboer. I (Giuliano Maggi) converted these lines from 
   Python (as made by Aart) to C++.

   These lines of code have a lack of const objects, this is on purpose to make it compatible 
   with what is written in ShowerFit.hh and ShowerPdf.hh
*/

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cassert>
#include <utility> 

#include <boost/lexical_cast.hpp>

#include "JDetector/JModuleRouter.hh"
#include "JDetector/JDetector.hh"
#include "JDAQ/JDAQEvent.hh"
#include "JDAQ/JDAQHit.hh"
#include "JDAQ/JDAQTriggeredHit.hh"
#include "JFit/JEvt.hh"
#include "JFit/JFitStatus.hh"
#include "JFit/JHistory.hh"
#include "JLang/JSharedPointer.hh"

#include "ShowerFit.hh"

#include "evt/Hit.hh"


namespace KM3ONLINE
{
  class AANetShowerFit
  {
    //-----In ctor list
    JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> router_;

    Det det_;
    
    //-------will be initialized in ctor
    MestShowerPdf * mpdf1_;  
    DigitalShowerPdf * digPdf_;
    
    ShowerFit mest_fit_;
    ShowerFit digi_fit_;
    
    std::vector<Vec> icosahedronVec_;
    
    //**************************  
    //-----Other Private members
    //************************* 
    
    //--- Tools (no present in the pythonic version)
    
    template <class HIT>
    static bool SortHitsByTime(const HIT &hit1,const HIT &hit2)
    { return hit1.t < hit2.t; }
    
    
    static bool SortTrkByLik(const Trk &t1,const Trk &t2)
    { return -t1.lik < -t2.lik; }
    
    
    template<class Hit_t = Hit>
    void
    AANetShowerWorker(std::vector<Hit_t> &inputhits,
		      JFIT::JEvt &outFits) ; //<- definition  below. 
    //Inside of this one, 
    // other private tools (see below) are invoked
    
    //--- Other private tools (as in the pythonic version)
    
    template<class Hit_t = Hit>
    std::vector<Hit_t>
    greedy_merge(std::vector<Hit_t> &hits, 
		 const int gate = 300 ) 
    {
      std::vector<Hit_t> r;
      
      if (hits.size()<2) return hits;
      
      unsigned int i=0;
      while ( i < hits.size() ){
	double T= hits.at(i).t+gate;
	
	for (unsigned int j=i+1; j< (hits.size()+1); j++){
	  
	  if ( j==hits.size() or hits.at(j).t > T ){
	    r.push_back(hits.at(i));
	    i=j;
	    break;
	  }
	  else{
	    hits.at(i).a += hits.at(j).a;
	    T = hits.at(j).t + gate;
	  }
	}//for
      }//while
      
      return r;
    }    
    
    
    template<class Hit_t = Hit>
    std::vector<Hit_t>
    merge_hits_per_pmt(std::vector<Hit_t> &hits) 
    {
      std::vector<Hit_t> r;
      
      std::map<int,std::vector<Hit_t> > M;
      for (auto h= hits.begin(); h != hits.end(); ++h)
	M[h->pmt_id].push_back(*h); 
      
      for (auto k = M.begin(); k!=M.end(); k++){
	std::vector<Hit_t> shits = k->second; 
	std::sort(shits.begin(),shits.end(),SortHitsByTime<Hit_t>);
	
	std::vector<Hit_t> v= this->greedy_merge(shits);
	for (auto h = v.begin(); h!=v.end(); h++) { r.push_back(*h); }
      }
      
      return r;
    } 
    
    /*      
	    Comments about function below : get_coincidences - Giuliano
	    
	    In the pythonic-version,
	    the line "if sv[k].type == -1 : h.type = -1"  has been suppressed here, 
	    since 'type' is a field for MC only
    */
    
    template<class Hit_t = Hit>
    std::vector<Hit_t>
    get_coincidences(std::vector<Hit_t> &hits, 
		     const unsigned int level= 2,
		     const unsigned int dt= 20 ) 
    {
      std::vector<Hit_t> r;
      
      std::unordered_map<int,std::vector<Hit_t> > M;
      for (auto h =hits.begin(); h != hits.end(); h++)
	M[h->dom_id].push_back(*h); 
      
      for (auto it= M.begin(); it != M.end(); it++){
	std::vector<Hit_t> sv=it->second;
	std::sort(sv.begin(),sv.end(),SortHitsByTime<Hit_t>);
	
	unsigned int i=0;
	while( i < sv.size() ){
	  
	  unsigned int j=i+1;
	  while( j < sv.size() and sv.at(j).getT() - sv.at(i).getT() < dt )   { j+=1; }
	  
	  if ( j > i+level-1 ){
	    Hit_t h= sv.at(i);
	    
	    for (unsigned int k=i+1 ; k < j; k++){ h.a += sv[k].a; }
	    r.push_back(h);
	    i=j;
	  }
	  else{
	    i+=1;
	  }
	  
	}//while
      }//for
      
      return r;
    }
    
    
    template<class Hit_t = Hit>
    std::vector<Hit_t>
    filter_hit_residuals(std::vector<Hit_t> &hit_vector, 
			 const Trk &trk,
			 const std::pair<double,double> &x= std::make_pair(-100,900)) 
    {
      std::vector<Hit_t> r;
      
      const double low_x = x.first;
      const double up_x  = x.second;
      const double number(217552605.0e-9);
      
      const double xtrack=trk.pos.x;
      const double ytrack=trk.pos.y;
      const double ztrack=trk.pos.z;
      
      for (auto h= hit_vector.begin(); h!= hit_vector.end(); h++){
	const double x= h->pos.x - xtrack; 
	const double y= h->pos.y - ytrack; 
	const double z= h->pos.z - ztrack; 
	
	const double len= std::sqrt(x*x + y*y + z*z);
	const double res = h->t-trk.t - (len/number);
	
	if (res > low_x and res < up_x) r.push_back(*h); 
      }
      return r;
    }
    
    
    void correct_energy(Trk &trk) 
    {
      trk.fitinf.push_back( trk.E );
      
      if ( trk.E > 10 and trk.E < 1e10 ){
	const double x = std::log10(trk.E);
	const double f = 0.53972 + x * 0.16419 + x*x * -0.0168034;
	
	if (f > 1e-3) trk.E = trk.E / f;
      }
      
    }
    
    
  public:
    
    AANetShowerFit(const JLANG::JSharedPointer<const JDETECTOR::JModuleRouter> &router,
		   const std::string detFileName,
		   const std::string pdfFile):
      router_(router)
    {
      det_.read(detFileName);
      
      //set icosahedronVec_
      const double a = ( sqrt(5.0) - 1 ) / 2.0;
      foreach( s, pack(-1,1) ) foreach( f, pack(-a,a) )
	{
	  icosahedronVec_.push_back( Vec( 0, s, f ) );
	  icosahedronVec_.push_back( Vec( s, f, 0 ) );
	  icosahedronVec_.push_back( Vec( f, 0, s ) );
	}
      
      //set pdfs ...
      mpdf1_= new MestShowerPdf();
      mpdf1_->verb=1;
      
      mest_fit_= {"mest1",mpdf1_};
      mest_fit_.fix_vars(3,4,6);
      
      digPdf_= new DigitalShowerPdf();
      digPdf_->infile     = pdfFile;
      digPdf_->k40prob    = 5000*1e-6;
      digPdf_->eval_mode  = digPdf_->quick;
      digPdf_->init();
      
      digi_fit_= {"digi",digPdf_};
      digi_fit_.verb =1;
      digi_fit_.fix_vars( 0,1,2,5 );
      digi_fit_.minimizer_name ="Minuit2";
      digi_fit_.minimizer_algo ="Migrad";
      
    }
    
    
    ~AANetShowerFit()
    {
      delete mpdf1_;
      delete digPdf_;
    }
    
    void
    getJEvt(const std::vector<KM3NETDAQ::JDAQSnapshotHit> &snapHits,
	    JFIT::JEvt &out) 
    {
      std::vector<Hit> aaHits;
      for (auto h = snapHits.begin(); h != snapHits.end(); ++h){
	Hit aaHit;

	aaHit.dom_id     = h->getModuleID();
	aaHit.channel_id = h->getPMT();
	aaHit.tot        = static_cast<unsigned int>(h->getToT());
	aaHit.t          = static_cast<double>(h->getT());
	aaHit.a          = 0;

	const JDETECTOR::JModule &om= router_->getModule( h->getModuleID() );

	const int pmt_id = static_cast<int>(h->getPMT());
	const JDETECTOR::JPMT &pmt= om.getPMT(pmt_id);
	const JGEOMETRY3D::JDirection3D &pmtDir=pmt.getDirection();
	const JGEOMETRY3D::JPosition3D &pmtPos=pmt.getPosition();

	aaHit.pos = Vec(pmtPos.getX(), pmtPos.getY(), pmtPos.getZ());
	aaHit.dir = Vec(pmtDir.getDX(), pmtDir.getDY(), pmtDir.getDZ());

	aaHits.push_back(aaHit);
      }

      this->AANetShowerWorker(aaHits,out);
      std::reverse(out.begin(), out.end()); //<- to be aligned with JPP, where the best fit is the first element
    }
    
  };

}  

template<class Hit_t>
void
KM3ONLINE::AANetShowerFit::AANetShowerWorker(std::vector<Hit_t> &InputHits,
					     JFIT::JEvt &outfits) 
{
  det_.apply(InputHits);
  
  const double hit_t0 = std::min_element(InputHits.begin(),InputHits.end(),
					 SortHitsByTime<Hit_t>)->t;
  
  for (auto h =InputHits.begin(); h != InputHits.end(); h++) { h->t -= hit_t0; }
  
  std::vector<Hit_t> input_hits= InputHits;  
  
  std::vector<Hit_t> hits= this->merge_hits_per_pmt(input_hits);
  
  for (auto h =hits.begin(); h != hits.end(); h++) { h->a = 1; } //h.a = 1 this pythonic-line is odd, - Giuliano
  
  std::vector<Hit_t> coin_hits = this->get_coincidences(hits,2,20);
  
  Hit_t pivot_hit;
  for (auto h = coin_hits.begin(); h != coin_hits.end(); h++)
    if ( h->a > pivot_hit.a ) pivot_hit=*h;
  
  Trk T0;
  const double estart(5e5);
  T0.pos= pivot_hit.pos;
  T0.t  = pivot_hit.t;
  T0.E  = estart;
  
  
  std::vector<Hit_t> mest_hits = 
    this->filter_hit_residuals(coin_hits ,T0, std::make_pair(-800,800)); 
  
  Trk T1 = mest_fit_.fit( T0, det_, mest_hits, 1 );
  
  //digi_fit_.pdf.verb = 0;
  digi_fit_.verb     = 0; 
  
  std::vector<Hit_t> digi_hits = 
    this->filter_hit_residuals(input_hits ,T1); 
  
  
  T0.comment  = "start";
  T1.comment  = "mest";
  
  const int rec_type_aanet=101;
  T0.rec_type = rec_type_aanet;
  T1.rec_type = rec_type_aanet;
  
  T0.id=1;
  T1.id=2;
  
  //T0.rec_stage=1;
  //T1.rec_stage=2;
  
  std::vector<Trk> outfits_aanet{T0,T1};
  
  digPdf_->force_precompute=true;
  
  std::vector<Trk> fitlist;
  
  int id=0;
  for (std::vector<Vec>::iterator dir= icosahedronVec_.begin();
       dir != icosahedronVec_.end(); ++dir){
    
    T1.dir.set(dir->x,dir->y,dir->z);
    
    Trk fit = digi_fit_.fit(T1,det_ ,digi_hits,1);
    std::string comment= boost::lexical_cast<std::string>(id);
    fit.comment="fit"+comment;
    
    fitlist.push_back(fit);
    id++;
  }
  
  std::sort(fitlist.begin(), fitlist.end(), SortTrkByLik);
  
  JFIT::JHistory history(JFIT::JSHOWERBEGIN);
  unsigned int i=0;
  for (std::vector<Trk>::iterator t= fitlist.begin(); t != fitlist.end(); ++t){
    t->id = i+2;
    //if ( fitlist.size()==i+1 ) t->rec_stage = 4 ;
    //else t->rec_stage = 3 ;
    //t->rec_stage  = rec_type_aanet;
    
    this->correct_energy(*t);
    
    t->fitinf.clear();
    t->fitinf.push_back ( digi_hits.size() );
    
    const JFIT::JFit jFit(history,
			  t->pos.x, t->pos.y, t->pos.z,
			  t->dir.x, t->dir.y, t->dir.z,
			  t->t, t->lik, digi_hits.size(), t->E,
			  JFIT::JFitStatus_t::OKAY);
    
    outfits.push_back(jFit);
    i++;
  }
}

  
#endif
