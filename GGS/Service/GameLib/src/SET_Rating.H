// $Id: SET_Rating.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Rating.H (bof) (c) Igor Durdanovic

#ifndef SET_Rating_H
#define SET_Rating_H

#include "SET.H"
#include "VC_Rating.H"

class String;

class SET_Rating
  : public SET<VC_Rating>
{
public:
  VC_Rating& operator () ( const String& K );

  void remove_empty();
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const
  {
    const_cast<SET_Rating*>(this)->remove_empty();
    return SET<VC_Rating>::save( os );
  }
  std::istream& load ( std::istream& is )
  {
    SET<VC_Rating>::load( is ); remove_empty();
    return is;
  }
  
private:
};

inline std::ostream& operator << ( std::ostream& os, const SET_Rating& v )
{
  return v.print( os );
}

#endif

//: SET_Rating.H (eof) (c) Igor
