// $Id: SET_PTR_Request.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_PTR_Request.H (bof) (c) Igor Durdanovic

#ifndef SET_PTR_Request_H
#define SET_PTR_Request_H

#include "SET_PTR.H"

class    Request;
class VC_Request;
class SET_String;

class SET_PTR_Request
  : public SET_PTR< VC_Request >
{
public:
  typedef bool (*COMPARE) ( const VC_Request& A, const Request& B  );

  /* */ VC_Request* find ( const Request& B, COMPARE f ); // sequential find
  const VC_Request* find ( const Request& B, COMPARE f ) const;

  std::ostream& print( std::ostream& os ) const;

  void erase( bool Free ) { SET_PTR< VC_Request >::erase( Free ); }
  void erase( const SET_String& Set );
  
private:
};

inline std::ostream& operator << ( std::ostream& os, const SET_PTR_Request& v )
{
  return v.print( os );
}

#endif

//: SET_PTR_Request.H (eof) (c) Igor
