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
//: SET_PTR.cc (bof) (c) Igor Durdanovic

#include <algorithm>

template<class O> 
SET_PTR<O>::~SET_PTR()
{
  erase( true );
}

template<class O> 
template<class T> 
O* SET_PTR<O>::operator() ( const T& K )
{
  PTR<O>* po = SET< PTR<O> >::operator() ( K );
  if ( po == 0 ) return 0;
  return &**po;
}

template<class O> 
template<class T> 
const O* SET_PTR<O>::operator() ( const T& K ) const
{
  PTR<O>* po = SET< PTR<O> >::operator() ( K );
  if ( po == 0 ) return 0;
  return &**po;
}

//

template<class O> 
O* SET_PTR<O>::operator+= ( O* V )
{
  PTR<O>  p( V );
  PTR<O>* po = SET< PTR<O> >::operator += ( p );
  return &**po;
}

template<class O> 
template<class T>
O* SET_PTR<O>::operator-= ( const T& K )
{
  iterator it = lower_bound( this->begin(), this->end(), K, SET_less< PTR<O>, T >() );
  O* o = 0;
  if ( it != this->end() && *it == K ) { o = &**it; SET< PTR<O> >::erase( it ); }
  return o;
}

//

template<class O> 
void SET_PTR<O>::erase( bool Free )
{
  if ( Free ) { // must use index, elements might remove themselves!!
    for ( sint4 i = this->size(); --i >= 0; ) (*this)[i].free();
  }
  SET< PTR<O> >::erase();
}

//: SET_PTR.cc (eof) (c) Igor
