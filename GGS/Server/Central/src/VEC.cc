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
//: VEC.cc (bof) (c) Igor Durdanovic

#include <algorithm>

template<class O> void VEC<O>::erase()
{
  std::vector<O>::erase( this->begin(), this->end() );
}

template<class O> void VEC<O>::erase( const typename VEC::iterator& it )
{
  std::vector<O>::erase( it );
}

template<class O> void VEC<O>::erase( const typename VEC::iterator& it, const typename VEC::iterator& hi )
{
  std::vector<O>::erase( it, hi );
}

template<class O> VEC<O>& VEC<O>::unique()
{
  sort( this->begin(), this->end() ); // remove dups
  std::vector<O>::erase( std::unique( this->begin(), this->end() ), this->end() );
  return *this;
}

template<class O> std::ostream& VEC<O>::print( std::ostream& os ) const
{
  const_iterator it = this->begin();
  const_iterator hi = this->end();
  if ( it != hi ) { os << *it; for ( ; ++it != hi; ) os << ' ' << *it; }
  return os;
}

template<class O> std::ostream& VEC<O>::save( std::ostream& os ) const
{
  sint4 n = this->size();
  os.write( ccptr(&n), sizeof(n) );
  const_iterator it = this->begin();
  const_iterator hi = this->end();
  for ( ; it != hi; ++it ) (*it).save( os );
  return os;
}

template<class O> std::istream& VEC<O>::load( std::istream& is )
{
  erase();
  sint4 n = 0;
  is.read( cptr(&n), sizeof(n) );
  this->reserve( n );
  for ( O o; --n >= 0 ; ) { ; push_back( o ); this->back().load( is ); }
  return is;
}

//: VEC.cc (eof) (c) Igor
