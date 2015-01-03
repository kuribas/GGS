// $Id: VC_SEQ.C 160 2007-06-22 15:21:10Z mburo $
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
//: VC_SEQ.C (bof) (c) Igor Durdanovic

#include "VC_SEQ.H"
#include "System.H"
#include "Actors.H"
#include "IO_FILE.H"
#include "VT100.H"
#include "VAR_System.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

VC_SEQ::VC_SEQ( const String& Name, TYPE T )
  : t(T), g(0)
{
  TSTAT;
  
  switch ( t ) {
  case CHANN : g = vc_var.channs( Name ); break;
  case GROUP : g = vc_var.groups( Name ); break;
  default    :
    vc_con << VCFL << "Sequence[" << int(t) << "] '" << Name << "' not found." << EOM;
  }
}

void VC_SEQ::text( const IO_TCP_Client* from, const String& name,
		   ostringstream& osML, ostringstream& os1L )
{
  TSTAT;
  
  if (! ok() ) return;

  SET_String::iterator it = g->obj.begin();
  SET_String::iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0    ) continue;
    if ( ip == from ) continue;
    if ( ip->ignore( from ) ) continue;
    ip->interrupt();
    switch ( t ) {
    case CHANN : ip->bell( ip->var.bell.tell_chann() ); break;
    case GROUP : ip->bell( ip->var.bell.tell_group() ); break;
    default    : vc_con << VCFL;
    }
    if ( from != 0 ) *ip << name << ' ';
    ip->text( ip->var.groups( group_service ) != 0 ? os1L : osML );
    ip->ready().sync();
    if ( from->var.groups( group_service ) == 0 ) ip->var.vars = Alias( var_lrecv, *g );
  }
}

//: VC_SEQ.C (eof) (c) Igor
