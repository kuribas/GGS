// $Id: TSTAT_Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: TSTAT_Client.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "USEC.H"
#include "TSTAT_Server.H"
#include "TSTAT_Client.H"
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

TSTAT_Client::TSTAT_Client( ccptr Name, ccptr File, sint4 Line )
{
  if (! (ok = tstat_ready) ) return;

  name  = Name;
  file  = File;
  line  = Line;
  utime = System::user_time();
  rtime = System::real_time();
  count = 0;
}

TSTAT_Client::~TSTAT_Client()
{
  if (! tstat_ready ) return;
  if (! ok ) return; ok = false;
  
  utime = System::user_time() - utime;
  rtime = System::real_time() - rtime;
  ++count;
  
  vc_tstat += *this;
}
  
void TSTAT_Client::accumulate ( const TSTAT_Client& C )
{
  utime += C.utime;
  rtime += C.rtime;
  count += C.count;
}

ostream& TSTAT_Client::print( ostream& os ) const
{
  os << setw(12) << USEC( rtime ) << ' '
     << setw(12) << USEC( utime ) << " / "
     << setw( 6) << count << " = "
     << setw( 5) << USEC( count ? rtime / count : 0 ) << ' '
     << setw( 5) << USEC( count ? utime / count : 0 ) << ' '
     << file << " : " << setw(3) << line << " : " << name;
    
  return os;
}

bool operator < ( const TSTAT_Client& a, const TSTAT_Client& b )
{
  int r = strcmp( a.file, b.file );
  if ( r < 0 ) return true;
  if ( r > 0 ) return false;
  if ( a.line < b.line ) return true;
  if ( a.line > b.line ) return false;
  return strcmp( a.name, b.name ) < 0;
}

bool operator ==( const TSTAT_Client& a, const TSTAT_Client& b )
{
  return ( strcmp( a.file, b.file ) == 0 &&
	   a.line == b.line &&
	   strcmp( a.name, b.name ) == 0 );
}

//: TSTAT_Client.C (eof) (c) Igor
