// $Id: SET.H 9037 2010-07-06 04:05:44Z mburo $
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
//: SET.H (bof) (c) Igor Durdanovic

#ifndef SET_H
#define SET_H

#include <vector>
#include <iostream>

template <class O>
class SET
  : public std::vector<O>
{
public:
  virtual ~SET() {}

  typedef typename std::vector<O>::      iterator       iterator;
  typedef typename std::vector<O>::const_iterator const_iterator;
  
  template<class T> /* */ O* operator () ( const T& K );
  template<class T> const O* operator () ( const T& K ) const;
  /*                   */ O* operator += ( const O& V );
  template<class T>     void operator -= ( const T& K );

  void erase();
  void erase( const SET::iterator& it );
  void erase( const SET::iterator& it, const SET::iterator& hi );

  void cross( const std::vector<O>& a, const std::vector<O>& b );
  void diff ( const std::vector<O>& a, const std::vector<O>& b );

  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
};

template<class O> 
std::ostream& operator << ( std::ostream& os, const SET<O>& v ) { return v.print( os ); }

#include "SET.cc"

#endif

//: SET.H (eof) (c) Igor
