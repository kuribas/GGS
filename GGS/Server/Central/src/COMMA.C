// $Id: COMMA.C 160 2007-06-22 15:21:10Z mburo $
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
//: COMMA.C (bof) (c) Igor Durdanovic

#include "COMMA.H"
#include "String.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& COMMA::print( ostream& os ) const 
{
  TSTAT;
  
  char buff[ 128 ]; // any uint8 should fit here
  sint8 t = ( n < 0 ? -n : n );
  sint4 c = 0;
  sint4 i = 127;
  buff[ i ] = 0;
  for ( ;; ) {
    if ( c == 3 )
      { buff[ --i ] = ','; c = 0; }
    buff[ ++c,--i ] = char('0' + (t % 10LL));
    if ( (t /= 10LL) == 0 ) break;
  }
  if ( n < 0 ) buff[--i] = '-';
  return os << String(buff + i);
}

//: COMMA.C (eof) (c) Igor
