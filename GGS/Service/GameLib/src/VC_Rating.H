// $Id: VC_Rating.H 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Rating.H (bof) (c) Igor Durdanovic
//  Abstract Game Rating class using Glicko system

#ifndef VC_Rating_H
#define VC_Rating_H

#include "CPP.H"
#include "Actors.H"
#include "String.H"
#include <iostream>

struct Rating
{
public:
  Rating() { erase(); }

  void erase();

  /*  */ real8 Rank() const { return rank; }
  /*  */ real8 Rdev() const;
  static real8 Rdev( real8 rdev, sint4 last, sint4 now );

  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

  static void rate( real8 NResult, real8& r1, real8& d1, real8& r2, real8& d2 );
  static void rate( real8  Result,
		    real8 NResult,
		    Rating&    R1,
		    Rating&    R2,
		    sint4    Date );

  bool empty() const { return date == 0; }
  
protected:
  real8 rank;
  real8 rdev;
  real8  sum; // needed to calculate average result
  sint4  win; 
  sint4 draw;
  sint4 loss;
  sint4 date; // date of the last update
};

inline std::ostream& operator << ( std::ostream& os, const Rating& v ) { return v.print( os ); }

//

class VC_Rating
  : public Rating
{
public:
  VC_Rating( const String& ID = str_empty ) : tid(ID) { erase(); }

  const String& id() const { return tid; }
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  String tid; // type id

  friend class VAR_Client; // need this to rate IOS games
};

inline bool operator <  ( const VC_Rating& A, const VC_Rating& B )
{
  return A.id()< B.id();
}
inline bool operator == ( const VC_Rating& A, const VC_Rating& B )
{
  return A.id()==B.id();
}
inline bool operator <  ( const VC_Rating& A, const String& B    )
{
  return A.id()< B;
}
inline bool operator == ( const VC_Rating& A, const String& B    )
{
  return A.id()==B;
}
inline std::ostream& operator << ( std::ostream& os, const VC_Rating& v )
{
  return v.print( os );
}

#endif

//: VC_Rating.H (eof) (c) Igor
