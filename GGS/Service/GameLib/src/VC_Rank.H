// $Id: VC_Rank.H 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Rank.H (bof) (c) Igor Durdanovic

#ifndef Rank_H
#define Rank_H

#include "CPP.H"
#include "VEC.H"
#include "Actors.H"
#include "String.H"
#include "VC_Rating.H"
#include "TIME_Client.H"
#include <iostream>

class Client;

class Rank
  : public Rating
{
public:
  Rank( const String Name = str_empty ) : name(Name), tdev( Rdev() ) {}

  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

private:
  String name;
  real8  tdev;

  friend class VC_Rank;

  friend bool lt_rank( const Rank& R1, const Rank& R2 );
  friend bool eq_rank( const Rank& R1, const Rank& R2 );
  friend bool lt_name( const Rank& R1, const Rank& R2 );
  friend bool eq_name( const Rank& R1, const Rank& R2 );
};

inline std::ostream& operator << ( std::ostream& os, const Rank& v ) { return v.print( os ); }

//

class VC_Rank
  : public TIME_Client
{
public:
  VC_Rank( const String& ID = "?" );
  VC_Rank( const VC_Rank& R );

  VC_Rank& operator=( const VC_Rank& R );

  virtual ~VC_Rank();

  size_t size() const { return by_name.size(); }
  
  const String& id() const { return tid; }
  
  void  top( Client& P, const String& ot, sint4 No = 16, sint4 From = 0 ) const;
  void rank( Client& P, const String& ot, const String& name ) const;

  void update( const String& id, const Rating& Rat );
  
  void erase();

  virtual void     te_handle( sint4 mssg, uint4 time );
  virtual std::ostream& te_print ( std::ostream& os ) const;

  std::ostream& print( std::ostream& os, bool vt100, const String& Name, sint4 Lo, sint4 No ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

protected:
  String tid;
  real8  avg; // average rating
  real8  dev; // standard deviation 

  VEC<Rank> by_name;
  VEC<Rank> by_rank;

private:
  static ccptr tag;

  void sort_check() const;
};

inline bool operator< (const VC_Rank& A, const VC_Rank& B){return A.id()< B.id();}
inline bool operator==(const VC_Rank& A, const VC_Rank& B){return A.id()==B.id();}
inline bool operator< (const VC_Rank& A, const String& B  ){return A.id()< B;}
inline bool operator==(const VC_Rank& A, const String& B  ){return A.id()==B;}

#endif

//: VC_Rank.H (eof) (c) Igor
