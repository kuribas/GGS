// $Id: SET_Rating.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Rating.C (bof) (c) Igor Durdanovic

#include "String.H"
#include "SET_Rating.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

VC_Rating& SET_Rating::operator () ( const String& K )
{
  TSTAT;
  
  VC_Rating* R = SET<VC_Rating>::operator() ( K );
  if ( R == 0 ) R = ( (*this) += VC_Rating( K ) );
  return *R;
}

void SET_Rating::remove_empty()
{
  for ( int i = size(); --i >= 0; ) {
    if ( (*this)[i].empty() ) erase( begin() + i );
  }
}

ostream& SET_Rating::print( ostream& os ) const
{
  TSTAT;

  const_cast<SET_Rating*>(this)->remove_empty();
  
  const_iterator it = begin();
  const_iterator hi = end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << EOL << *it; }
  
  return os;
}

//: SET_Rating.C (eof) (c) Igor
