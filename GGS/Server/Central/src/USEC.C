// $Id: USEC.C 160 2007-06-22 15:21:10Z mburo $
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
//: USEC.C (bof) (c) Igor Durdanovic

#include "HHMMSS.H"
#include "USEC.H"
#include "String.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& USEC::print( ostream& os ) const 
{
  TSTAT;
  
  sint8 ts = ( us < 0 ? -us : us );
  sint8 sec  = us / uSec;
  sint8 msec = ts % uSec; msec /= 1000;
  int w = os.width() - 4; if ( w < 0 ) w = 0; os << setw(w);
  os << HHMMSS( sec ) << '.';
  Form( os, "%03lld", msec );
  return os;
}

bool USEC::parse( const String& S, bool Min )
{
  TSTAT;
  
  HHMMSS hms;
  String p1, p2;
  String::parse( S, p1, p2, '.' );

  if (! hms.parse( p1, Min ) ) return false;

  if ( p2.empty() ) us = 0; else if (! sint8_parse( p2, false, us ) ) return false;

  us += hms.sec() * uSec;

  return true;
}

ostream& USEC::save ( ostream& os ) const
{
  os.write( ccptr(&us), sizeof(us) );
  return os;
}

istream& USEC::load ( istream& is )
{
  is.read( cptr(&us), sizeof(us) );
  return is;
}

//: USEC.C (eof) (c) Igor
