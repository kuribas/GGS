// $Id: Histogram_HDMY.C 9037 2010-07-06 04:05:44Z mburo $
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
//: Histogram_HDMY.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "Histogram_HDMY.H"
#include <time.h>
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const String Histogram_HDMY::tag( "Histogram_HDMY" );
				  
using namespace std;

Histogram_HDMY::Histogram_HDMY()
  : _H(6),
    _D(24),
    _M(31),
    _Y(12)
{
  TSTAT;
}
  
ostream& Histogram_HDMY::print( ostream& os, TYPE T ) const
{
  TSTAT;
  
  static ccptr hour[ 6] = { "00", "10", "20", "30", "40", "50" };
  static ccptr year[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  switch ( T ) {
  case HOURLY  : _H.print( os, hour ); break;
  case DAILY   : _D.print( os       ); break;
  case MONTHLY : _M.print( os       ); break;
  case YEARLY  : _Y.print( os, year ); break;
  default      : vc_log << VCFL; Form( vc_log, "T(%d)", T ) << endl << VCER;
  }

  return os;
}

void Histogram_HDMY::update( int Val )
{
  TSTAT;
    
  time_t c = ::time(0);
  struct tm* tc = localtime( &c );
  
  int Hi = tc->tm_min / 10;
  int Di = tc->tm_hour;
  int Mi = tc->tm_mday - 1;
  int Yi = tc->tm_mon;

  _H.update( Hi, Val );
  _D.update( Di, Val );
  _M.update( Mi, Val );
  _Y.update( Yi, Val );
}

ostream& Histogram_HDMY::save ( ostream& os ) const
{
  TSTAT;

  tag.tag_save( os );
  
  _H.save( os );
  _D.save( os );
  _M.save( os );
  _Y.save( os );

  return os;
}

istream& Histogram_HDMY::load ( istream& is )
{
  TSTAT;

  if (! tag.tag_chck( is ) ) { vc_con << VCFL; System::exit(-1); }

  _H.load( is );
  _D.load( is );
  _M.load( is );
  _Y.load( is );

  return is;
}
  

//: Histogram_HDMY.C (eof) (c) Igor
