// $Id: SET_PTR.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_PTR.H (bof) (c) Igor Durdanovic

#ifndef SET_PTR_H
#define SET_PTR_H

#include "PTR.H"
#include "SET.H"

template <class O>
class SET_PTR
  : public SET< PTR<O> >
{
public:
  ~SET_PTR();

  typedef typename SET< PTR<O> >::      iterator       iterator;
  typedef typename SET< PTR<O> >::const_iterator const_iterator;
  
  template<class T> /* */ O* operator() ( const T& K );
  template<class T> const O* operator() ( const T& K ) const;
  /*                   */ O* operator+= ( /* */ O* V );
  template<class T> /* */ O* operator-= ( const T& K );
  template<class T> /* */ O* erase      ( const T& K );
  
  void erase( bool Free );
  
protected:
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
};

template<class O> 
std::ostream& operator << ( std::ostream& os, const SET_PTR<O>& v )
{
  return v.print( os );
}

#include "SET_PTR.cc"

#endif

//: SET_PTR.H (eof) (c) Igor
