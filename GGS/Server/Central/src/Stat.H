// $Id: Stat.H 160 2007-06-22 15:21:10Z mburo $
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
//: Stat.H (bof) (c) Igor Durdanovic

#ifndef Stat_H
#define Stat_H

#include "CRatio.H"
#include "System.H"
#include "TIME_Client.H"
#include <iostream>

class IO_Stat
{
public:
  IO_Stat() : cur(0), max(0), sum(0) {}

  IO_Stat& operator += ( sint4 n ) { sum += n; return *this; }

  sint8 current() const { return cur; }
  
  void update( real8 Sec );

  std::ostream& print( std::ostream& os ) const;

protected:
  sint8 cur;
  sint8 max;
  sint8 sum;
  sint8 tot;
};

inline std::ostream& operator << ( std::ostream& os, const IO_Stat& v ) { return v.print(os); }

//

class N_Stat
{
public:
  N_Stat() : cur(0), max(0), tot(0) {}

  N_Stat& operator ++ () { ++tot; if ( ++cur > max ) max = cur; return *this; }
  N_Stat& operator -- () {        if ( --cur < min ) min = cur; return *this; }

  sint4 current() const { return cur; }

  std::ostream& print( std::ostream& os ) const;

protected:
  sint4 cur;
  sint4 min;
  sint4 max;
  sint4 tot;
};

inline std::ostream& operator << ( std::ostream& os, const N_Stat& v ) { return v.print(os); }

//

class Stat
  : public TIME_Client
{
public:
  Stat();
  virtual ~Stat();

  virtual void          te_handle( sint4 Mssg, uint4 Time );
  virtual std::ostream& te_print ( std::ostream& os ) const;

  std::ostream& print( std::ostream& os ) const;

  sint8   io_time; // when was it created
  sint8   io_last; // when did we calculated flow last time
  IO_Stat io_inp;
  IO_Stat io_out;
  CRatio  io;      // io stat
  CRatio  db;      // db stat
  N_Stat  n_user;
};

inline std::ostream& operator << ( std::ostream& os, const Stat& v ) { return v.print(os); }

#endif

//: Stat.H (eof) (c) Igor
