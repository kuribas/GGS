// $Id: HHMMSS.C 160 2007-06-22 15:21:10Z mburo $
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
//: HHMMSS.C (bof) (c) Igor Durdanovic

#include "VEC.H"
#include "String.H"
#include "HHMMSS.H"
#include <iomanip>
#include <sstream>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& HHMMSS::print( ostream& os ) const 
{
  TSTAT;
  
  int w = os.width(); os << setw(0);

  ostringstream ts;
  const int MM = 60;
  const int HH = 60 * MM;
  const int DD = 24 * HH;
  sint8 r = ( s < 0 ? -s : s );
  int dd = r / DD; r %= DD;
  int hh = r / HH; r %= HH;
  int mm = r / MM; r %= MM;
  int ss = r;
  if ( s < 0 ) ts << '-';
  if ( dd )                       ts << dd << '.';
  if ( dd || hh || w >= 8 )       Form( ts, "%02d:" , hh );
  if ( dd || hh || mm || w >= 5 ) Form( ts, "%02d:" , mm );
  /*                           */ Form( ts, "%02d"  , ss );
  const string& s = ts.str();
  for ( ; w > int(s.size()); --w ) os << ' ';
  os.write( s.data(), s.size() );
  return os;
}

bool HHMMSS::parse( const String& S, bool Min )
{
  TSTAT;
  
  VEC<String> sx; String::parse( S, sx, ':' );

  switch ( sx.size() ) {
  case 1 : {
    if (! sint8_parse( sx[0], false, s ) ) return false;
    if ( Min ) s *= 60;
  } return true;
  case 2 : {
    sint8 m;
    if (! sint8_parse( sx[0], false, m ) ) return false;
    if (! sint8_parse( sx[1], false, s ) ) return false;
    s += m * 60;
  } return true;
  case 3 : {
    sint8 h,m;
    if (! sint8_parse( sx[0], false, h ) ) return false;
    if (! sint8_parse( sx[1], false, m ) ) return false;
    if (! sint8_parse( sx[2], false, s ) ) return false;
    s += m * 60 + h * 3600;
  } return true;
  }
  
  return false;
}

//: HHMMSS.C (eof) (c) Igor
