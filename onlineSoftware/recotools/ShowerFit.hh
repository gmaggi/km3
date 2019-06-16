#ifndef SHOWER_FIT_INCLUDE
#define SHOWER_FIT_INCLUDE

/**                                                                                                             
   The author of these lines of code is Aart Heijboer. These is copied in order to make the Cpp implementation  
    of some python components of the AANetShower reco.                                                          
*/

#include "evt/Evt.hh"
#include "showrec_util.hh"
#include "ShowerPdf.hh"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include "TStopwatch.h"
#include "Fit/FitConfig.h"

namespace KM3ONLINE
{
struct ShowerFit 
{
  int verb;
  string name;
  double total_time;
  int    nfits;

  // --------------------------
  // The Pdf to use in the fit
  //---------------------------
  ShowerPdfBase* pdf;

  // --------------------------
  // which paramters to fix
  //---------------------------
  vector<int> fixed_vars;
  void fix_vars( int v1, int v2=-1, int v3=-1, int v4=-1, int v5=-1, int v6=-1, int v7=-1 )
  {
    fixed_vars = pack( v1,v2,v3,v4,v5,v6,v7 );
  }

 //-------------------------------------------
  // minimizer and recording controll
  //-------------------------------------------
  string minimizer_name;
  string minimizer_algo;
  vector<Trk> path_;
  bool        record;
  double      tollerance;
  
  void _init()
  {
    nfits          = 0;
    total_time     = 0;
    verb           = 1; 
    record         = false;
    fixed_vars     = pack( 6 );
    tollerance     = 1e-2;
    minimizer_name = "GSLMultiMin"; //Minuit2
    minimizer_algo = "BFGS2";       //Migrad
  }

  ShowerFit( string name_, string pdfname )
  {
    name = name_;
    pdf = 0;
    if (pdfname == "mest" ) pdf = new MestShowerPdf();
    if (pdfname == "digi" ) pdf = new DigitalShowerPdf();
                              
    if (!pdf) fatal ("unknown pdf", pdfname);
    _init();

  }

  ShowerFit(string name_, ShowerPdfBase* pdf_ )
  {
    name = name_;
    pdf = pdf_;
    _init();
  }

  ShowerFit()
  {
    _init();
  }

  Trk fit( Trk& start_track , 
	   Det& det, 
           vector<Hit>& data, 
	   int fitmode /*=1*/ );

  // -- the fitting functions --
  double score_function( const double* pars ) // delegate to pdf->eval
  {
    static Trk trk;
    to_trk( pars, trk );
    return pdf -> eval( trk );
  }

  //ClassDef( ShowerFit, 1 )
};

}

void fit_errorhandler (int level, Bool_t abort, const char *location,
                       const char *msg)
{
  if (level < 1001 ) return;
  cout << " There was a root error. level=" << level << endl;

  if (level < 2001 ) return;
  cout << " stack trace will follow and then we'll exit " << endl;
  cout << endl << endl;
  gSystem->StackTrace();
  exit(1);
  return;
}


Trk KM3ONLINE::ShowerFit::fit( Trk& start_track ,
			       Det& det,
			       vector<Hit>& data,
			       int fitmode /*=1*/ )
{
  TStopwatch W;

  path_.clear();
  pdf -> init_event( det, data );

  ROOT::Math::Minimizer* M =
    ROOT::Math::Factory::CreateMinimizer( minimizer_name.c_str(), minimizer_algo.c_str());

  M->SetMaxFunctionCalls(10000);   // for Minuit/Minuit2                                                       
                         
  M->SetMaxIterations(100);        // for GSL                                                                   
                          
  M->SetTolerance(0.001);
  M->SetPrintLevel( verb );


  ROOT::Math::Functor f( this , &ShowerFit::score_function, 7 );
  M->SetFunction( f );

  double vstart[7];
  to_pars( start_track, vstart );
  vstart[5]+=0.1; // starting at 0 seems to fix parameter                                                       
                          
  vector<double> step = pack(1.0 ,1.0 ,1.0 , 0.01, 0.01, 1.0, 0.1 );

  if (verb) print ("fixing ", fixed_vars , fitmode , verb );

  for (int i=0; i<7; i++ )
    {
      if ( contains( fixed_vars, i ) )
        {
          M -> SetFixedVariable( i, parnames()[i] ,vstart[i] );
        }
      else
        {
          M -> SetVariable( i, parnames()[i] ,vstart[i], step[i] );
        }
    }

  ErrorHandlerFunc_t oldhandler = SetErrorHandler( fit_errorhandler );

  M -> Minimize(); // for great justice!                                                                        
                          

  SetErrorHandler( oldhandler );

  Trk r;
  to_trk( M -> X(), r );
  r.lik = M -> MinValue();

  r.fitinf.resize(1);
  r.fitinf[0] = W.RealTime();

  total_time += r.fitinf[0];
  nfits++;

  //r.rec_stage = M -> Status();


  r.error_matrix.resize(49);
  M -> GetCovMatrix( &r.error_matrix[0] );

  delete M;

  return r;
}

#endif
