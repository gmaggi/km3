#include "evt/Trk.hh"
#include "util/Table.hh"

// --------------------------------------------------
// functions for converting between double[] and Trk.
// in aid of fitting.
//---------------------------------------------------

namespace KM3ONLINE
{
inline vector<string> parnames()
{
  static vector<string> r;
  if (r.size()==0) r = split(string("x y z theta phi t logE"));
  return r;
}

inline void to_pars( Trk& t, double* r )
{
  r[0] = t.pos.x; r[1] = t.pos.y; r[2] = t.pos.z;
  r[3] = acos( t.dir.z );
  r[4] = atan2( t.dir.y, t.dir.x );
  r[5] = t.t;
  r[6] = log10(t.E);
}

inline void to_trk( const double* r, Trk& t )
{
  t.pos.set( r[0], r[1], r[2] );
  t.dir.set_angles( r[3],r[4] );
  t.t = r[5]; 
  t.E = pow(10,r[6]);
}

inline void print_error_matrix( Trk& t , ostream& out = cout )
{
  
  vector<string> s = parnames();
  s.insert(s.begin(), " x ");
  Table T( s );
  int N = parnames().size();

  vector<double> scaler = pack( 1.0,1,1, 180/M_PI, 180/M_PI, 1);

  enumerate( j, pn, parnames() )
    {
      T << pn;
      for( int i=0; i<N; i++)
	{
	  double v = t.error_matrix[i+N*j];
	  v*=scaler[i];
	  v*=scaler[j];

	  T << v;
	}
    }
  
  T.print_ascii( out );
  

}

}
