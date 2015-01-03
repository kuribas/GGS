// $Id: rate.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#include <cmath>
#include <cassert>
#include <stdlib.h>
#include <iostream>

typedef double real8;

void rate( real8 NResult, real8& r1, real8& d1, real8& r2, real8& d2 )
{
  const real8 p = 0.00001007241561756780; // 3*ln(10)^2/(pi^2*400^2)
  const real8 q = 0.00575646273248511421; //   ln(10)/400
  const real8 min_K = 16.0;
    
  real8 d12 = r1 - r2;
  real8 sd1 = d1 * d1;
  real8 sd2 = d2 * d2;

  {
    real8 f = 1.0 / sqrt( 1 + p * sd2 );
    real8 E = 1.0 / ( 1.0 + pow( 10.0, -d12 * f / 400.0 ) );
    real8 k = 1.0 / sd1 + q*q * f*f * E * (1.0 - E);
    real8 K = q*f / k;
    if ( K < min_K ) K = min_K;
    r1 = r1 + 2 * K * ( NResult - E );
    d1 = 1.0 / sqrt( k );
  }
  NResult = 1.0 - NResult; // invert result
  {
    real8 f = 1.0 / sqrt( 1 + p * sd1 );
    real8 E = 1.0 / ( 1.0 + pow( 10.0, d12 * f / 400.0 ) );
    real8 k = 1.0 / sd2 + q*q * f*f * E * (1.0 - E);
    real8 K = q*f / k;
    if ( K < min_K ) K = min_K;
    r2 = r2 + 2 * K * ( NResult - E );
    d2 = 1.0 / sqrt( k );
  }
}

int main( int ac, char** av )
{
  if ( ac < 6 ) { cerr << "nr r1 d1 r2 d2" << endl; abort(); }

  real8 nr = atof( av[1] );
  real8 r1 = atof( av[2] );
  real8 d1 = atof( av[3] );
  real8 r2 = atof( av[4] );
  real8 d2 = atof( av[5] );
  rate( nr, r1, d1, r2, d2 );
  cerr.form( "%7.2f@%6.2f %7.2f@%6.2f", r1,d1,r2,d2 ) << endl;
  return 0;
}
