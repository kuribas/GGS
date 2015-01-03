// $Id: COMMA.H 160 2007-06-22 15:21:10Z mburo $
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
//: COMMA.H (bof) (c) Igor Durdanovic

#ifndef COMMA_H
#define COMMA_H

#include "CPP.H"

class COMMA
{
public: 
  COMMA( sint8 N ) : n(N) {}

  std::ostream& print( std::ostream& os ) const;

protected:
  sint8 n;
};

inline std::ostream& operator << ( std::ostream& os, const COMMA& v ) { return v.print(os); }

#endif

//: COMMA.H (eof) (c) Igor
