// $Id: VC_Rating.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, igord@research.nj.nec.com
    NEC Research Institute
    4 Independence Way
    Princeton, NJ 08540, USA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//: VC_Rating.C (bof) (c) Igor Durdanovic

#include "HHMMSS.H"
#include "System.H"
#include "VC_Rating.H"
#include <iomanip>
#include <cmath>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void Rating::erase()
{
  TSTAT;
  
  rank = 1720.0;
  rdev =  350.0;
  sum  = 0;
  win  = 0;
  draw = 0;
  loss = 0;
  date = 0;
}

#if 0
real8 Rating::Rdev( real8 rdev, sint4 last, sint4 now )
{
  TSTAT;
  
  static const real8 t_rdev = 100.0;                   // additional rating deviation
  static const real8 t_norm = 60.0 * 60.0 * 24.0 * 30; // time unit
  static const real8 t_year = 12.0;                    // period = 12 * time_unit
  static const real8 c      = t_rdev * t_rdev / log( 1.0 + t_year );
  /*        */ real8 t      = now < last ? 0 : now - last;
  
  real8 tdev = sqrt( rdev * rdev + c * log( 1.0 + t / t_norm ) );
  return tdev > 350.0 ? 350.0 : tdev;
}
#else
real8 Rating::Rdev( real8 rdev, sint4 last, sint4 now )
{
  TSTAT;

  static const real8 t_rdev = 350.0; // additional rating deviation
  static const real8 t_norm = 60.0 * 60.0 * 24 * 365 * 5;
  static const real8 c      = t_rdev / sqrt(t_norm);
  /*        */ real8 t      = now < last ? 0 : now - last;

  real8 tdev = sqrt( rdev * rdev + c * c * t );
  return tdev > 350.0 ? 350.0 : tdev;
}
#endif

real8 Rating::Rdev() const
{
  TSTAT;
  
  return Rdev( rdev, date, System::clock() );
}

void Rating::rate( real8 NResult, real8& r1, real8& d1, real8& r2, real8& d2 )
{
  TSTAT;
  
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

void Rating::rate( real8  Result,
		   real8 NResult,
		   Rating&    R1,
		   Rating&    R2,
		   sint4    Date )
{
  TSTAT;
  
  /**/ if ( Result > 0 ) { R1.win++;  R2.loss++; }
  else if ( Result < 0 ) { R1.loss++; R2.win++;  }
  else                   { R1.draw++; R2.draw++; }
  R1.sum += Result;
  R2.sum -= Result;

  R1.rdev = Rdev( R1.rdev, R1.date, Date ); // deviation = old dev + time dev
  R2.rdev = Rdev( R2.rdev, R2.date, Date );
  R1.date = Date;
  R2.date = Date;
  rate( NResult, R1.rank, R1.rdev, R2.rank, R2.rdev );
};

ostream& Rating::print ( ostream& os ) const // top & rank
{
  TSTAT;
  
  sint4 num = win + draw + loss;
  real8 avg = num == 0 ? 0 : sum / num;
  Form( os, "%6.1f@%5.1f=", rank, Rdev() );
  if ( date == 0 ) os << String( " ----------- " );
  else os << setw(13) << HHMMSS( System::clock() - date );
  Form( os, "+@%5.1f%+6.1f%7d%7d%7d", rdev, avg, win, draw, loss );
  return os;
}

ostream& Rating::save ( ostream& os ) const
{
  TSTAT;
  
  os.write( ccptr(&rank), sizeof(rank) );
  os.write( ccptr(&rdev), sizeof(rdev) );
  os.write( ccptr(&sum),  sizeof(sum) );
  os.write( ccptr(&win),  sizeof(win) );
  os.write( ccptr(&draw), sizeof(draw) );
  os.write( ccptr(&loss), sizeof(loss) );
  os.write( ccptr(&date), sizeof(date) );

  return os;
}

istream& Rating::load ( istream& is )
{
  TSTAT;
  
  is.read( cptr(&rank), sizeof(rank) );
  is.read( cptr(&rdev), sizeof(rdev) );
  is.read( cptr(&sum),  sizeof(sum) );
  is.read( cptr(&win),  sizeof(win) );
  is.read( cptr(&draw), sizeof(draw) );
  is.read( cptr(&loss), sizeof(loss) );
  is.read( cptr(&date), sizeof(date) );

  return is;
}

//

ostream& VC_Rating::print ( ostream& os ) const // top & rank
{
  TSTAT;
  
  os << setw(-5) << id() << ' ';
  Rating::print( os );
  
  return os;
}

ostream& VC_Rating::save ( ostream& os ) const
{
  TSTAT;
  
  tid.save( os );
  Rating::save( os );

  return os;
}

istream& VC_Rating::load ( istream& is )
{
  TSTAT;
  
  tid.load( is );
  Rating::load( is );

  return is;
}

//: VC_Rating.C (eof) (c) Igor
