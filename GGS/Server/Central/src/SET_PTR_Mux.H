// $Id: SET_PTR_Mux.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_PTR_Mux.H (bof) (c) Igor Durdanovic

#ifndef SET_PTR_Mux_H
#define SET_PTR_Mux_H

#include "IO_TCP_Mux.H"
#include "SET_PTR.H"

class SET_PTR_Mux
  : public SET_PTR< IO_TCP_Mux >
{
public:
  void sync();

  std::ostream& print( std::ostream& os ) const;
};

inline std::ostream& operator << ( std::ostream& os, const SET_PTR_Mux& v )
{
  return v.print( os );
}

#endif

//: SET_PTR_Mux.H (eof) (c) Igor
