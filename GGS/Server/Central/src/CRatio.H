// $Id: CRatio.H 160 2007-06-22 15:21:10Z mburo $
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
//: CRatio.H (bof) (c) Igor Durdanovic

#ifndef CRatio_H
#define CRatio_H

#include "CPP.H"

class CRatio
{
public:
  CRatio( sint8 Txt = 0, sint8 Com = 0 ) : txt(Txt), com(Com) {}

  sint8 Txt() const { return txt; }
  sint8 Com() const { return com; }

  CRatio& operator += ( const CRatio& cr ) { txt += cr.txt; com += cr.com; return *this; }
  
  std::ostream& print( std::ostream& os, ccptr fmt = "%7.3f", sint4 cw = 14, sint4 tw = 14 ) const;
  
protected:
  sint8 txt;
  sint8 com;
};

inline std::ostream& operator << ( std::ostream& os, const CRatio& v ) { return v.print(os); }

#endif

//: CRatio.H (eof) (c) Igor
