// $Id: GAME_Stored.H 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_Stored.H (bof) (c) Igor Durdanovic

#ifndef GAME_Stored_H
#define GAME_Stored_H

#include "CPP.H"
#include "VEC.H"
#include "Actors.H"
#include "String.H"

class Stored
{
public:
  Stored( const String& Info    = str_empty,
	  const String& Counter = str_empty,
	  const String& Name1   = str_empty,
	  const String& Name2   = str_empty,
	  /* */ sint4   Date    = 0 )
    : info   (Info),
      counter(Counter),
      name1  (Name1),
      name2  (Name2),
      date(Date) {}

  const String& Counter() const { return counter; }
  const String& Name1  () const { return name1; }
  const String& Name2  () const { return name2; }
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  String info;
  String counter;
  String name1;
  String name2;
  sint4  date;

  friend class GAME_Stored;
};

inline std::ostream& operator << ( std::ostream& os, const Stored& v ) { return v.print( os ); }

//

class GAME_Stored
{
public:
  void add( const String& Info,
	    const String& Counter,
	    const String& Name1,
	    const String& Name2 );

  VEC<Stored>::const_iterator begin() const { return vec.begin(); }
  VEC<Stored>::const_iterator end()   const { return vec.end(); }
  
  uint4 size() const { return vec.size(); }

  void erase() { vec.erase(); }
  void erase( const String& Counter );

  bool          operator[] ( const String& C ) const; // counter
  sint4              count ( const String& P1, const String& P2 ) const;
  
  const Stored& operator[] ( uint4 idx       ) const { return vec[idx]; }
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  static ccptr tag;
  
  VEC<Stored> vec;
};

inline std::ostream& operator << ( std::ostream& os, const GAME_Stored& v )
{
  return v.print( os );
}

#endif

//: GAME_Stored.H (eof) (c) Igor
