// $Id: SET_Alias.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Alias.H (bof) (c) Igor Durdanovic

#ifndef SET_Alias_H
#define SET_Alias_H

#include "Alias.H"
#include "SET.H"
#include "VEC.H"

class SET_Alias
  : public SET<Alias>
{
public:
  SET_Alias() : _size(0) {}

  uint4 save_size() const { return _size; }
  
  virtual void erase();

  /*  */ bool replace_alias( String& Cmd, const SET_Alias* Next ) const;
  /*  */ void replace_var  ( String& Txt ) const;
  static void replace_arg  ( String& Txt, const String& Arg );

  Alias* operator += ( const Alias& A );
  void   operator -= ( const Alias& A );
  
  SET_Alias&    operator = ( const Alias&  a ); // (un)sets alias
  const String& def        ( const String& s ) const; // get alias definition

  std::ostream& print( std::ostream& os ) const;
  std::ostream& print( std::ostream& os,  const String& Prefix ) const;
  std::ostream& save ( std::ostream& os ) const; // will skip _alias
  std::istream& load ( std::istream& is );

private:
  static ccptr tag;
  
  uint4 _size; // # of aliases that will be actually saved
};

inline std::ostream& operator << ( std::ostream& os, const SET_Alias& v ) { return v.print( os ); }

#endif

//: SET_Alias.H (eof) (c) Igor
