#ifndef SHOWER_PDF_INCLUDE
#define SHOWER_PDF_INCLUDE

/**                                                                                                             
   The author of these lines of code is Aart Heijboer. These is copied in order to make the Cpp implementation 
   of some python components of the AANetShower reco.
*/

#include "evt/Evt.hh"
#include "evt/Det.hh"
#include "util/foreach.hh"

#include "TH3D.h"
#include "TFile.h"

namespace KM3ONLINE
{
struct ShowerPdfBase
{
  string name;
  int verb;

  virtual string desc() { return "ShowerPdfBase"; }
  virtual void init()   { cout << "ShowerPdfBase::init" << endl; }
  virtual void init_event( Det& det,
			   vector<Hit>& hits )
  {
    cout << "ShowerPdfBase::initev" << endl;
  }
  virtual double eval (Trk& t ) {return 0;}

  ShowerPdfBase() { verb=0; }
  virtual ~ShowerPdfBase(){}
};


//----------------------------------------------------
// M-estimator PDF
//----------------------------------------------------

struct MestShowerPdf : public ShowerPdfBase
{
  vector<Hit>* hits_;
  
  double constant;
  double power;

  string desc() { return "m-estimate";}

  MestShowerPdf() 
  {
    constant = 1;
    power    = 0.5;
    name = "MestShowerPdf";
  }

  void init() 
  {
    cout << " this is " << name << endl;
  }
  
  void init_event( Det& det, vector<Hit>& hits )
  {
    hits_ = &hits;
   
  }

  double eval( Trk& trk )
  {
    if (verb>1) dbg( trk.__str__() );
    
    double m =0;
    
    foreach( hit, *hits_ )
      {
        const double r  = (hit.pos - trk.pos).len();
        const double t = hit.t - trk.t - r / v_light;
        m += hit.a * pow( constant + t*t , power ); 
      }
   
    return m;
  }

  //ClassDef( MestShowerPdf, 1 )
};


//----------------------------------------------------
// Digital Poisson pdf based on P(hit/no hit)
//----------------------------------------------------


/*! Structure to store minimum information 
  needed to compute the likelihood. */


struct DomSum
{
  Vec pos;                  // dom position
  vector< Vec > hit_pmts;   // directions of pmts with a hit
  vector< Vec > unhit_pmts; // directions of the unhit pmts 
};

/*! Structure to store precomputed information for very
    fast evaluation of the likelihood vs energy and direction
    for a given position */
struct Fdom
{
  Fdom( const TH1D& h ) : hist0(h) {}
  Fdom() {}

  Vec dir;                 // normalized vector from shower to dom
  TH1D hist0;              // sum of z-slices (mu vs z) for unhit pmts 
  vector<TH1D> hit_hists;  // z-slices for the hit pmts
};

/*! A PDF representing the probablity to have either zero hits or more than one hit */

struct DigitalShowerPdf : public ShowerPdfBase
{
  double k40prob;
  double minlogp, maxlogp;
  double minp, maxp;
  
  bool dbg_probs;
  bool do_clamp;
  bool force_precompute; 

  // histograms for the PDF
  string infile;
  TH3D H3;
  TH3D G3;

  // event data
  vector<DomSum> domsums;
  vector<Fdom> fdoms; // pre-computed per-dom information for fast eval
  TH1D hz0;


  enum Eval_mode { plain, quick , hybrid , diagnostic};
  Eval_mode eval_mode;

  string desc() { return "pdf describing P(hit/nohit)";}
   
  DigitalShowerPdf() : dbg_probs(false)
  {
    name   = "DigitalShowerPdf";
    infile = "none";
    set_minmax_prob( 0.0001 );
    
    do_clamp = false;
    k40prob = 0.005; // for 5 khz rate and a 1microsecond window
    eval_mode = diagnostic;
  }
  
  void set_minmax_prob( double pmin )
  {
    minlogp = log( pmin );
    maxlogp = log( 1-pmin );
    minp = exp( minlogp );
    maxp = exp( maxlogp );
  }
  
  void init() 
  {
    TFile f( infile.c_str() );
   
    getobj( f, "h3xa" ,    H3 );
    getobj( f, "h3normx" , G3 );
    
    cout << H3.GetDirectory() << endl;
    H3.Divide(&G3);

    verb = true;

    if (false ) // for when H3 is not mu, but 1-P0   
      for(int ix = 1; ix < H3.GetNbinsX()+1; ix++)
	for(int iy = 1; iy < H3.GetNbinsY()+1; iy++)
	  for(int iz = 1; iz < H3.GetNbinsZ()+1; iz++)
	    {
	      int b = H3.GetBin(ix, iy, iz);          
	      double p0 = 1 - H3.GetBinContent( b );
	      double pp = p0;
	      double eps = 1e-30;
	      if (p0 < eps )     p0 = eps;
	      if (p0 > 1-eps)    p0 = 1-eps;
	      double mu = -log( p0 );
	      if (mu < 0 ) fatal ("highscool math failure.");
	      if ( verb > 0 ) print (" transform to mu",ix,iy,iz, "   ",  pp, p0, mu );
	      H3.SetBinContent( b, mu );
	    }
  }

  /*! Function to be called for every new event. Sets the domsums. */

  void init_event( Det& det, vector<Hit>& hits )
  {
    //force_precompute = true; 
    
    domsums.clear();
    foreach_map( id , pmtptr, det.pmts ) { (void)id; pmtptr->flag = 0; }
    foreach( h, hits ) 
      {
	if ( det.pmts.find (  h.pmt_id ) == det.pmts.end() )
	  {
	    fatal("cannot find pmt with id", h.pmt_id );
	  }
	  

	det.pmts[ h.pmt_id ] -> flag = 1;
      }
    foreach_map( domid, dom , det.doms )
      {
	(void)domid;
	DomSum D;
	D.pos = dom.pos;
	
	foreach( pmt, dom.pmts )
	  {
	    if (pmt.flag) D.hit_pmts.push_back( pmt.dir );
	    else          D.unhit_pmts.push_back( pmt.dir );
	  }
      
	domsums.push_back(D);
      }
  }


  double get_h3_value( TH3D& H, double x, double y, double z )
  {
    const int interpolation_policy = 1;
    
    switch( interpolation_policy ) 
      {
      case 0 :  // no interpolation, just bincontent
        {
          const int ix = H.GetXaxis() -> FindBin ( x );
          const int iy = H.GetYaxis() -> FindBin ( y );
          const int iz = H.GetZaxis() -> FindBin ( z );
          return H.GetBinContent( H.GetBin(ix,iy,iz) );
        }

      case 1: // trilinear interploation (by ROOT) 
        clamp_bincenter( x, H.GetXaxis() );
        clamp_bincenter( y, H.GetYaxis() );
        clamp_bincenter( z, H.GetZaxis() );
        return H.Interpolate( x,y,z );

      case 2:
	print ("spline interpolation not available in this verion");
        //return interpolate_TH3( H, x,y,z );

      default:
        fatal("wrong interpolation_policy", interpolation_policy);
      }
  }
 
  double eval( Trk& trk )
  {
    switch ( eval_mode )
      {
      case plain  : return eval_plain( trk );
      case quick   : return eval_quick( trk );
      case hybrid : return eval_hybrid( trk );
      case diagnostic :  return eval_diagnostic( trk );
      }
    fatal("unknown eval_mode");
  }
 

  double eval_diagnostic( Trk& trk )
  {
    double Lp = eval_plain( trk );
    double Lf = eval_quick( trk );
    double Lh = eval_hybrid( trk );
    print ("likelihoods ", Lp, Lf, Lh );
    return Lf;
  }

  double eval_plain( Trk& trk )
  {
    TIMER()
      
    if (trk.E > 1e14 ) trk.E = 1e14;

    const double alpha = trk.E * 1e-6; // energy scale factor ( H3 is for 1 PeV ).

    double L=0;
    double L1(0), L0(0);
    int nhit =0;

    foreach( dom, domsums )
      {
	const Vec v = dom.pos - trk.pos;
	const double r = v.len();
	const Vec dirv = v/r;

        const double z = trk.dir.dot( dirv );
	
	{ 
	  TIMER( hit-pmt-loop )
	  
	  foreach( pmtdir, dom.hit_pmts )
	    {
	      nhit ++;
	      const double a   = dirv.dot ( pmtdir );
	      const double mu1 = get_h3_value( H3, r,z,a );
	      const double mu  = alpha * mu1 + k40prob;
	      
	      double logp1 = log( 1 - exp( - mu ) );
	      
	      if (do_clamp) clamp( logp1, minlogp, maxlogp );
	      L -= logp1;
	      L1 -= logp1;
	    }
	}
	
	

	{ 
	  TIMER( unhit-pmt-loop )
      
	  foreach ( pmtdir, dom.unhit_pmts )
	    {
	      const double a   = dirv.dot ( pmtdir );
	      const double mu1 = get_h3_value( H3, r,z,a );
	      const double mu  = alpha * mu1 + k40prob;
	      
	      // note : - L = alpha * sum( mu1 )  + N*k40prob;
	      // can define, for all empty pmts on each dom:
	      // sum( f(z) ) = F(z) 
	      
	      double logp0 = -mu;               
	      if (do_clamp) clamp( logp0, minlogp, maxlogp );
	      L -= logp0;
	      L0 -=logp0;
	    }
	}
	
	if ( L != L ) 
	  {
	    print("likelihood is nan @ r,z,a =", r,z,0 );
	    print( trk.__str__() );
	  }
	
      } // dom

    return L;
  }

  bool precompute_if_needed( const Vec& pos )
  {
    static Vec olpos;
    
    if ( pos != olpos || force_precompute ) 
      {
	// cout << " precompute ! " << pos << " " << olpos << endl;
	precompute( pos );
	olpos = pos;
	force_precompute = false;
	return true;
      }
    return false;
  }

  
  void precompute( const Vec& pos ) // fill fdoms
  {
    TIMER("");
    print ("precompute________________________________");
    
    TH1::AddDirectory(0);
    
    // one prototype histogram (for axis clamping)
    hz0 = histogram_slice_y ( &H3, 100, 0.5 );
    hz0.Scale(0);

    fdoms.resize( domsums.size() , Fdom(hz0) );
    
    vector<double> va(31);

    enumerate( i, dom, domsums )
      {	
	Fdom& fdom = fdoms[i];

	const Vec v = dom.pos - pos;
	const double r = v.len();
	fdom.dir = v/r;
	
	//---------------------------------------------------------
	// for all unhit pmts, we make a histogram of sum(mu) vs z
	// per dom
	//---------------------------------------------------------
	
	
	// fill va.
	const int N = dom.unhit_pmts.size();
	va.resize( N );
	for (int ipmt=0; ipmt < N; ipmt++) va[ipmt] = dom.unhit_pmts[ipmt].dot ( fdom.dir );
	
	fdom.hist0.Scale(0);
	interpol_xz( &H3, r , va , &fdom.hist0 );
	
	// now the non-emtpy doms

	fdom.hit_hists.clear();
	foreach ( pmtdir, dom.hit_pmts )
	  {
	    const double a = pmtdir.dot ( fdom.dir );
	    fdom.hit_hists.push_back(  histogram_slice_y ( &H3, r, a ) );
	  }

      } // domsums loop
  }

 


  double eval_quick( Trk& trk )
  { 
    precompute_if_needed( trk.pos );
    TIMER();
    
    double L=0;
    //double L0=0;- Giuliano
    //double L1=0;- Giuliano
    
    if (trk.E > 1e14 ) trk.E = 1e14;
   
    const double alpha = trk.E * 1e-6; // energy scale factor ( H3 is for 1 PeV ).
    
    const int nterms      = 2; // do this many products before taking the log
    int termcounter = 0;
    double P = 1;
    int nhit = 0;
      
    foreach( fdom, fdoms )
      {
	double z = fdom.dir.dot( trk.dir );
	clamp_bincenter( z, hz0.GetXaxis());
       
	
	foreach( h, fdom.hit_hists )
	  {
	    nhit++;
	    const double mu = k40prob + alpha * interpolate( &h, z );
	    
	    double p = 1.0 - exp( -mu );
	    if (do_clamp) clamp( p, minp, maxp );
	    P *= p;
	    
	    
	    if ( ++termcounter == nterms )
	      {
		L  -= log(P);
		P = 1.0;
		termcounter = 0;
	      }	    
	  
	  if ( termcounter == nterms )
	    { 
	      L -= log(P); // last few terms.. if termcounter == 0 , P will be 1,
	      P = 1.0;
	      termcounter = 0;
	    }
	  }
      
	  // unhit pmts
	  const int nempty = 31 - fdom.hit_hists.size();
	  if (nempty)
	    { 
	      const double p = k40prob * nempty + alpha * interpolate( &fdom.hist0, z );
	      L += p;
	    }
	  
      }// fdomloop
    
    return L;
  }
    
  
  double eval_hybrid( Trk& trk )
  { 
    precompute_if_needed(trk.pos);

    TIMER();
    
    double L=0;
    double L0=0;
    double L1=0;
    if (trk.E > 1e14 ) trk.E = 1e14;
   
    const double alpha = trk.E * 1e-6; // energy scale factor ( H3 is for 1 PeV ).

    int nhit = 0;

    {      
      TIMER( Hit-pmts );

      foreach( dom, domsums )
	{
	  const Vec v = dom.pos - trk.pos;
	  const double r = v.len();
	  const Vec dirv = v/r;
	  
	  const double z = trk.dir.dot( dirv );
	  
	  foreach( pmtdir, dom.hit_pmts )
	    {
	      nhit ++;
	      const double a   = dirv.dot ( pmtdir );
	      const double mu1 = get_h3_value( H3, r,z,a );
	      const double mu  = alpha * mu1 + k40prob;
	      
	      double logp1 = log( 1 - exp( - mu ) );
	      
	      if (do_clamp) clamp( logp1, minlogp, maxlogp );
	      L -= logp1;
	      L1 -= logp1;
	    }
	}
      
    } // timer
    
    
    { 
      TIMER (empty-pmts )
      
	foreach( fdom, fdoms )
	{
	  const double z = trk.dir.dot( fdom.dir );
	  const int nempty = 31 - fdom.hit_hists.size();
	  double p = k40prob * nempty + alpha * interpolate( &fdom.hist0, z );
	  L += p;
	  L0 += p;
	}
    } // timer
    
    return L;
 }
  

};


struct Likresult
{
  double L; // total likelihood
  double L1; // likelihood of hit pmts
  double L0; // likelihood of unhit pmts
  

};

}//namespace

#endif
