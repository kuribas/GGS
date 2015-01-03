// $Id: PTR.H 160 2007-06-22 15:21:10Z mburo $
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
//: PTR.H (bof) (c) Igor Durdanovic
//: PTR is not the owner of the object!! so it will not deallocate

#ifndef PTR_H
#define PTR_H

#include <iostream>

class String;

template <class O>
class PTR
{
public:
  explicit PTR( O* P = 0 ) : p(P) {}

  void free() { delete p; p = 0; }

  /* */ O& operator*  () /* */ { return *p; }
  const O& operator*  () const { return *p; }
  /* */ O* operator-> () /* */ { return  p; }
  const O* operator-> () const { return  p; }

  std::ostream& print( std::ostream& os ) const {
    if ( p ) os << *p; else os << "[0]"; return os;
  }
  
private:
  O* p;
};

template<class O> std::ostream& operator << ( std::ostream& os, const PTR<O>& v )
{
  return v.print( os );
}

template <class O>          inline bool operator <  ( const PTR<O>& p, const PTR<O>& q )
{
  return *p <  *q;
}
template <class O>          inline bool operator == ( const PTR<O>& p, const PTR<O>& q )
{
  return *p == *q;
}
template <class O, class T> inline bool operator <  ( const PTR<O>& p, const T& q      )
{
  return *p <   q;
}
template <class O, class T> inline bool operator == ( const PTR<O>& p, const T& q      )
{
  return *p ==  q;
}

#endif

//: PTR.H (eof) (c) Igor
