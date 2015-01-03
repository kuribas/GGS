// $Id: TIME_Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: TIME_Client.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "TIME_Server.H"
#include "TIME_Client.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

uint4 TIME_Client::te_create( sint4 Mssg, uint4 Time, bool Abs )
{
  TSTAT;
  
  return clock = vc_time.add( this, Mssg, Time, Abs );
}

void TIME_Client::te_cancel( sint4 Mssg )
{
  TSTAT;
  
  vc_time.del( this, Mssg );
}

void TIME_Client::te_cancel()
{
  TSTAT;
  
  vc_time.del( this );
}

//: TIME_Client.C (eof) (c) Igor
