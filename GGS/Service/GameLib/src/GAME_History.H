// $Id: GAME_History.H 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_History.H (bof) (c) Igor Durdanovic

#ifndef GAME_History_H
#define GAME_History_H

#include "CPP.H"
#include "VEC.H"
#include "Actors.H"
#include "String.H"

class History
{
public:
  History( const String& Info    = str_empty,
	   const String& Counter = str_empty,
	   /* */ real8   Rank1   = 0,
	   const String& Name1   = str_empty,
	   /* */ real8   Rank2   = 0,
	   const String& Name2   = str_empty,
	   /* */ real8   Result  = 0,
	   /* */ sint4   Date    = 0 )
    : info(Info),
      counter(Counter),
      rank1(Rank1),
      name1(Name1),
      rank2(Rank2),
      name2(Name2),
      result(Result),
      date(Date) {}

  const String& Name1() const { return name1; }
  const String& Name2() const { return name2; }
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  String info;
  String counter;
  real8  rank1;
  String name1;
  real8  rank2;
  String name2;
  real8  result;
  sint4  date;

  friend class GAME_History;
};

inline std::ostream& operator << ( std::ostream& os, const History& v ) { return v.print( os ); }

//

class GAME_History
{
public:
  GAME_History( uint4 Len ) : len(Len) {}

  String add( const String& Info,
	      const String& Counter,
	      /* */ real8   Rank1,
	      const String& Name1,
	      /* */ real8   Rank2,
	      const String& Name2,
	      /* */ real8   Result,
	      /* */ sint4   Date );

  bool operator() ( const String& Counter ) const;

  VEC<History>::const_reverse_iterator rbegin() const { return vec.rbegin(); }
  VEC<History>::const_reverse_iterator rend()   const { return vec.rend(); }
  
  uint4 size() const { return vec.size(); }

  void erase() { vec.erase(); }
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  uint4        len;
  VEC<History> vec;

private:
  static const String tag;
};

inline std::ostream& operator << ( std::ostream& os, const GAME_History& v )
{
  return v.print( os );
}

#endif

//: GAME_History.H (eof) (c) Igor
