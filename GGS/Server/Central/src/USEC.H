// $Id: USEC.H 160 2007-06-22 15:21:10Z mburo $
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
//: USEC.H (bof) (c) Igor Durdanovic

#ifndef USEC_H
#define USEC_H

#include "CPP.H"

class USEC
{
public:
  USEC( sint8 USec = 0) : us(USec) {}
  
  sint8  sec() const    { return us / uSec; }
  sint8 usec() const    { return us; }
  sint8 uadd( sint8 u ) { return us += u; }
  sint8 uset( sint8 u ) { return us  = u; }
  
  bool          parse( const String& S, bool Min = false );
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
protected:
  sint8 us;
};

inline bool          operator == ( const USEC& u1, const USEC& u2 )
{
  return u1.usec() == u2.usec();
}
inline std::ostream& operator << ( std::ostream& os,    const USEC& v  )
{
  return v.print(os);
}

#endif

//: USEC.H (eof) (c) Igor
