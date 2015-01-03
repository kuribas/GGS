// $Id: GAME_Clock.H 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_Clock.H (bof) (c) Igor Durdanovic

#ifndef GAME_Clock_H
#define GAME_Clock_H

#include <iostream>

#include "USEC.H"

// syntax: [ini[,[N]m1]]/[inc[,[N]m2]]/[ext[,[N]m3]]
//  ini = initial time
//    N = if timeout happens game is NOT lost (we go into extension)
//   m1 = #moves that has to be made within ini, 0 = whole game, ext is loss
//  inc = increment time
//    N = increment is NOT additive
//   m2 = #moves after which increment time is added, m2 > 0
//  ext = extension time added if clock runs out (m1=0) or m1 moves played
//    N = extension is NOT additive
//   m3 = #moves before next extension, 0 = whole game

// othello: 15:00,0/0,1/2:00,0,loss
// chess:   2:0:0,40/0,1/30:0,0
// go:      5:0,20/0,1/5:0,20

class GAME_Clock
{
public:
  GAME_Clock();

  void init();                       // default settings
  
  bool parse( const String& arg, bool in_init = false ); // this will read the settings!!

  void  reset();                     // resets clock to settings
  void  start();
  sint8 stop ( sint8 Delta_Since_Start = -1 ); // in usec; -1 = use wall clock
  sint8 event( sint8 Delta_Since_Event = -1 ); // in usec; -1 = use wall clock
  sint8 elapsed_since_start() const;
  sint8 elapsed_since_event() const;
  
  bool running() const { return ticking; }
  
  bool soft_timeout() const { return timeout_soft; }
  bool hard_timeout() const { return timeout_hard; }

  sint4 Ini()      const { return ini_set.sec(); }
  sint4 Ini_no()   const { return m1; }
  bool  Ini_loss() const { return ini_loss; }
  
  sint4 Inc()     const { return inc_set.sec(); }
  sint4 Inc_no()  const { return m2; }
  sint4 Inc_add() const { return inc_add; }

  sint4 Ext()     const { return ext_set.sec(); }
  sint4 Ext_no()  const { return m3; }
  sint4 Ext_add() const { return ext_add; }
  
  std::ostream& print( std::ostream& os, bool Setting = false ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
  sint8 time_left( bool setting = false ) const;

private:
  bool timeout_soft;
  bool timeout_hard;
  bool ticking;

  bool ini_loss; // hard timeout
  bool inc_add;
  bool ext_add;

  USEC ini_set;
  USEC inc_set;
  USEC ext_set;

  sint4 m1, m2, m3;

  // dynamic

  bool in_extension;
  
  USEC now; // this is the current (left) time
  USEC inc;
  USEC ext;
  USEC mark_start;
  USEC mark_event;
  USEC time_event;
  USEC inc_sum;
  
  sint4 c1, c2, c3;

  //

  void  update( sint8 Delta_Since_Event ); // this updates clock's state
  
  friend bool operator == ( const GAME_Clock& c1, const GAME_Clock& c2 ) {
    bool eq =
      c1.ini_set == c2.ini_set &&
      c1.inc_set == c2.inc_set &&
      c1.ext_set == c2.ext_set;
    return eq;
  }
};


#endif

//: GAME_Clock.H (eof) (c) Igor
