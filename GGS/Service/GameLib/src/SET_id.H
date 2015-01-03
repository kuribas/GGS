// $Id: SET_id.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_id.H (bof) (c) Igor Durdanovic

#ifndef SET_id_H
#define SET_id_H

#include "CPP.H"
#include <vector>

template <int min_len = 8>
class SET_id
{ // produces new id's and recycles old
public:
  SET_id();

  sint4 get();
  void  put( sint4 id );

private:
  std::vector<sint4> free;
  sint4              max;
};

#include "SET_id.cc"

#endif

//: SET_id.H (eof) (c) Igor
