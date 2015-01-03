// $Id: VEC.H 160 2007-06-22 15:21:10Z mburo $
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
//: VEC.H (bof) (c) Igor Durdanovic

#ifndef VEC_H
#define VEC_H

#include "CPP.H"
#include <vector>

template <class O>
class VEC
  : public std::vector<O>
{
public:
  typedef typename std::vector<O>::      iterator       iterator;
  typedef typename std::vector<O>::const_iterator const_iterator;
  
  void erase();
  void erase( const VEC::iterator& it );
  void erase( const VEC::iterator& it, const VEC::iterator& hi );

  VEC& unique();
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
};

template<class O> 
std::ostream& operator << ( std::ostream& os, const VEC<O>& v ) { return v.print( os ); }

#include "VEC.cc"

#endif

//: VEC.H (eof) (c) Igor
