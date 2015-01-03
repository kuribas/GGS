// $Id: GAME_Clock.C 160 2007-06-22 15:21:10Z mburo $
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
//: GAME_Clock.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "String.H"
#include "HHMMSS.H"
#include "GAME_Clock.H"
#include "RegularBoardGame.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void GAME_Clock::init()
{
  in_extension = false;
  now  = 0;
  inc  = 0;
  ext  = 0;
  mark_start = 0;
  mark_event = 0;
  time_event = 0;
  
  ini_loss = true;
  inc_add  = true;
  ext_add  = true;

  m1 = 0;
  m2 = 1;
  m3 = 0;
  
  bool ok = parse( RegularBoardGame::DEFAULT_CLOCK, true );
  assert( ok );
}

GAME_Clock::GAME_Clock()
{
  init();
}

bool GAME_Clock::parse( const String& s, bool in_init )
{
  TSTAT;

  String arg, tmp, rst(s), m, t;

  if (! in_init ) init();

  String::parse( rst, arg, tmp, '/' ); rst = tmp;
  String::parse( arg, t, m, ',' );
  if (!t.empty() && !ini_set.parse( t, true )) return false;
  if (!m.empty()) {
    if ( toupper(m[0]) == 'N' ) { ini_loss = false; m.erase( 0, 1 ); }
    if (!sint4_parse( m, false, m1 )) return false;
  }
  if ( rst.empty() ) return true;

  String::parse( rst, arg, tmp, '/' ); rst = tmp;
  String::parse( arg, t, m, ',' );
  if (!t.empty() && !inc_set.parse( t, true )) return false;
  if (!m.empty()) {
    if ( toupper(m[0]) == 'N' ) { inc_add = false; m.erase( 0, 1 ); }
    if ( !sint4_parse( m, false, m2 ) ) return false;
    if ( m2 < 1 ) return false;
  }
  if ( rst.empty() ) return true;

  String::parse( rst, t, m, ',' );
  if (!t.empty() && !ext_set.parse( t, true )) return false;
  if (!m.empty()) {
    if ( toupper(m[0]) == 'N' ) { ext_add = false; m.erase( 0, 1 ); }
    if ( !sint4_parse( m, false, m3 ) ) return false;
  }
  
  return true;
}

sint8 GAME_Clock::time_left( bool setting ) const
{
  TSTAT;
  
  if (setting) return ini_set.usec();
  else {
    if ( ticking ) return now.usec() - elapsed_since_event();
    else           return now.usec();
  }
}
  
void GAME_Clock::reset()
{
  TSTAT;
  
  c1 = c2 = c3 = 0;
  now.uset( ini_set.usec() );
  inc.uset( inc_set.usec() );
  ext.uset( ext_set.usec() );
  ticking      = false;
  inc_sum      = 0;
  in_extension = false;
  timeout_soft = timeout_hard = false;
}

void GAME_Clock::start()
{
  TSTAT;
  
  if ( ticking ) return;
  
  ticking = true;
  sint8 time_now = System::real_time();
  mark_start.uset( time_now );
  mark_event.uset( time_now );
  time_event = 0;
}

sint8 GAME_Clock::elapsed_since_start() const
{
  return System::real_time() - mark_start.usec();
}

sint8 GAME_Clock::elapsed_since_event() const
{
  return System::real_time() - mark_event.usec();
}

// < 0: ERROR

void GAME_Clock::update( sint8 Delta_Since_Event )
{
  TSTAT;

  assert( Delta_Since_Event >= 0 );
  
  inc_sum.uadd( Delta_Since_Event );
  now.    uadd(-Delta_Since_Event );

  ++c2; if ( in_extension ) { ++c3; } else { ++c1; } // advance counters

  if ( now.usec() < 0 ) { // time out rule
    if ( in_extension ) {
      timeout_soft = timeout_hard = true;
      return;
    } else {
      if ( m1 == 0 ) {
	if ( ini_loss ) timeout_soft = true;
	if ( ext.usec() == 0 ) timeout_soft = timeout_hard = true;
      } else {
	if ( ini_loss ) timeout_soft = timeout_hard = true;
      }
    }
  }
  
  if ( !in_extension ) { //  number of moves reached     time out
    if ( (m1 != 0 && c1 >= m1) || (now.usec() < 0) ) { // ini -> ext rule
      if ( ext_add ) { now.uadd( ext.usec() ); } else { now.uset( ext.usec() ); }
      c3 = 0;
      in_extension = true;
    }
  } else /*         */ { //  number of moves reached
    if ( m3 != 0 && c3 >= m3 ) { // ext -> ext rule
      if ( ext_add ) { now.uadd( ext.usec() ); } else { now.uset( ext.usec() ); }
      c3 = 0;
    }
  }

  if ( c2 >= m2 ) { // increment rule
    if ( inc_add ) {
      now.uadd( inc.usec() );
    } else {
      if ( inc_sum.usec() >= inc.usec() ) inc_sum.uset( inc.usec() );
      now.uadd( inc_sum.usec() );
    }
    c2 = 0;
    inc_sum = 0;
  }
}

sint8 GAME_Clock::stop( sint8 Delta_Since_Start )
{
  TSTAT;

  if (!ticking) return -1;

  sint8 delta = Delta_Since_Start < 0 ? elapsed_since_start() : Delta_Since_Start;
  delta -= time_event.usec(); // time that has already been accounted for
  if ( delta < 0 ) delta = 0;

  ticking = false;

  update( delta );

  return delta;
}

sint8 GAME_Clock::event( sint8 Delta_Since_Event )
{
  TSTAT;
  
  if (!ticking) return -1;

  sint8 delta = Delta_Since_Event < 0 ? elapsed_since_event() : Delta_Since_Event;
  if ( delta < 0 ) delta = 0;
  mark_event.uset( System::real_time() );
  time_event.uadd( delta );

  update( delta );

  return delta;
}

ostream& GAME_Clock::print( ostream& os, bool Setting ) const
{
  TSTAT;
  
  if ( Setting ) {
    os << setw(5) << HHMMSS( ini_set.sec() );
    if ( m1 != 0 || !ini_loss ) {
      os << ',';
      if (!ini_loss ) os << 'N';
      if ( m1 != 0  ) os << m1;
    }
    os << '/';
    if ( inc_set.sec() != 0 ) {
      os << setw(5) << HHMMSS( inc_set.sec() );
      if ( m2 != 1 || !inc_add ) {
	os << ',';
	if (!inc_add ) os << 'N';
	if ( m2 != 1 ) os << m2;
      }
    }
    os << '/';
    os << setw(5) << HHMMSS( ext_set.sec() );
    if ( m3 != 0 || !ext_add ) {
      os << ',';
      if (!ext_add ) os << 'N';
      if ( m3 != 0 ) os << m3;
    }
  } else {
    os << setw(5) << HHMMSS( time_left() / uSec );
    os << ',';
    if (!ini_loss ) os << 'N';
    os << c1 << ':' << m1;
    os << '/';
    if ( inc.sec() != 0 ) {
      os << setw(5) << HHMMSS( inc_set.sec() );
      os << ',';
      if (!inc_add ) os << 'N';
      os << c1 << ':' << m2;
    }
    os << '/';
    os << setw(5) << HHMMSS( ext.sec() );
    os << ',';
    if (!ext_add ) os << 'N';
    os << c1 << ':' << m3;
  }
  return os;
}

ostream& GAME_Clock::save( ostream& os ) const
{
  os.write( ccptr(&timeout_soft), sizeof(timeout_soft) );
  os.write( ccptr(&timeout_hard), sizeof(timeout_hard) );
  
  os.write( ccptr(&ini_loss), sizeof(ini_loss) );
  os.write( ccptr(&inc_add),  sizeof(inc_add) );
  os.write( ccptr(&ext_add),  sizeof(ext_add) );
  ini_set.save( os );
  inc_set.save( os );
  ext_set.save( os );
  os.write( ccptr(&m1), sizeof(m1) );
  os.write( ccptr(&m2), sizeof(m2) );
  os.write( ccptr(&m3), sizeof(m3) );
  os.write( ccptr(&in_extension), sizeof(in_extension) );
  now.save( os );
  inc.save( os );
  ext.save( os );
  os.write( ccptr(&c1), sizeof(c1) );
  os.write( ccptr(&c2), sizeof(c2) );
  os.write( ccptr(&c3), sizeof(c3) );

  return os;
}

istream& GAME_Clock::load( istream& is )
{
  init();
  
  is.read( cptr(&timeout_soft), sizeof(timeout_soft) );
  is.read( cptr(&timeout_hard), sizeof(timeout_hard) );
  
  is.read( cptr(&ini_loss), sizeof(ini_loss) );
  is.read( cptr(&inc_add),  sizeof(inc_add) );
  is.read( cptr(&ext_add),  sizeof(ext_add) );
  ini_set.load( is );
  inc_set.load( is );
  ext_set.load( is );
  is.read( cptr(&m1), sizeof(m1) );
  is.read( cptr(&m2), sizeof(m2) );
  is.read( cptr(&m3), sizeof(m3) );
  is.read( cptr(&in_extension), sizeof(in_extension) );
  now.load( is );
  inc.load( is );
  ext.load( is );
  is.read( cptr(&c1), sizeof(c1) );
  is.read( cptr(&c2), sizeof(c2) );
  is.read( cptr(&c3), sizeof(c3) );

  return is;
}

//: GAME_Clock.C (eof) (c) Igor
