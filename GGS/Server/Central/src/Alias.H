// $Id: Alias.H 160 2007-06-22 15:21:10Z mburo $
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
//: Alias.H (bof) (c) Igor Durdanovic

#ifndef Alias_H
#define Alias_H

#include "String.H"
#include "NAMED.H"

class Alias
  : public NAMED<String>
{ 
public:
  /*    */ Alias() {}
  explicit Alias( const String& S ) { String::parse( S, *this, obj ); }
  /*    */ Alias( const String& A, const String& D ) : NAMED<String>(A,D) {}

  static bool alias_name_ok( const String& s );
  static bool   var_name_ok( const String& s );
  
  void          def(  const String& Def ) { obj = Def; }
  const String& def() const { return obj; }

  void erase() { NAMED<String>::erase(); obj.erase(); }

  std::ostream& print( std::ostream& os ) const;
  std::ostream& print( std::ostream& os, const String& Prefix ) const;
};

inline std::ostream& operator << ( std::ostream& os, const Alias& v ) { return v.print( os ); }

#endif

//: Alias.H (eof) (c) Igor
