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
//: SET.cc (bof) (c) Igor Durdanovic

#include <algorithm>
#include "CPP.H"

template<class T1, class T2>
class SET_less
{
public:
  bool operator() ( const T1& a, const T2& b ) { return a < b; }
};

//

template<class O> 
template<class T> 
O* SET<O>::operator () ( const T& K )
{
  iterator it = lower_bound( this->begin(), this->end(), K, SET_less< O, T >() );
  if ( it != this->end() && *it == K  ) return & (*it);
  return 0;
}

template<class O> 
template<class T> 
const O* SET<O>::operator () ( const T& K ) const
{
  const_iterator it = lower_bound( this->begin(), this->end(), K, SET_less< O, T >() );
  if ( it != this->end() && *it == K ) return & (*it);
  return 0;
}

//

template<class O> 
O* SET<O>::operator += ( const O& V )
{
  iterator it = lower_bound( this->begin(), this->end(), V, SET_less< O, O >() );
  if ( it == this->end() || !( *it == V) ) return &(*insert( it, V ));
  return &(*it);
}

template<class O> 
template<class T> 
void SET<O>::operator -= ( const T& K )
{
  iterator it = lower_bound( this->begin(), this->end(), K, SET_less< O, T >() );
  if ( it != this->end() && *it == K ) erase( it );
}

//

template<class O> void SET<O>::erase()
{
  std::vector<O>::erase( this->begin(), this->end() );
}

template<class O> void SET<O>::erase( const iterator& it )
{
  std::vector<O>::erase( it );
}

template<class O> void SET<O>::erase( const iterator& it, const iterator& hi )
{
  std::vector<O>::erase( it, hi );
}

template<class O> void SET<O>::cross( const std::vector<O>& a, const std::vector<O>& b )
{
  erase();
  set_intersection(a.begin(), a.end(), b.begin(), b.end(),
		   std::back_insert_iterator< SET >( *this ) );
}

template<class O> void SET<O>::diff( const std::vector<O>& a, const std::vector<O>& b )
{
  erase();
  set_difference(a.begin(), a.end(), b.begin(), b.end(),
		 std::back_insert_iterator< SET >( *this ) );
}

//

template<class O> std::ostream& SET<O>::print( std::ostream& os ) const
{
  const_iterator it = this->begin();
  const_iterator hi = this->end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << ' ' << *it; }
  return os;
}

template<class O> std::ostream& SET<O>::save( std::ostream& os ) const
{
  sint4 n = this->size();
  os.write( ccptr(&n), sizeof(n) );
  const_iterator it = this->begin();
  const_iterator hi = this->end();
  for ( ; it != hi; ++it ) (*it).save( os );
  return os;
}

template<class O> std::istream& SET<O>::load( std::istream& is )
{
  erase();
  sint4 n = 0;
  is.read( cptr(&n), sizeof(n) );
  for ( O o ; --n >= 0 ; ) { o.load( is ); (*this) += o; }
  return is;
}

//: SET.cc (eof) (c) Igor
