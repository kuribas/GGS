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
//: SET_id.C (bof) (c) Igor Durdanovic

#include "SET_id.H"

template <int min_len>
SET_id<min_len>::SET_id()
{
  max = 8;
  for ( sint4 i = 0; i < max; i++ ) free.push_back( i );
}

template <int min_len>
sint4 SET_id<min_len>::get()
{
  if ( free.size() < min_len ) { // expand
    sint4 i = max; max += 8;
    for ( ; i < max; i++ ) free.push_back( i );
  }
  sint4 i = free[0];
  free.erase( free.begin() );
  return i;
}

template <int min_len>
void SET_id<min_len>::put( sint4 i )
{
  free.push_back( i );
}

//: SET_id.C (eof) (c) Igor
