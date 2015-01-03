// $Id: EXE_Client_ignore.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client_ignore.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "IO_TCP_Client.H"
#include "VC_SEQ.H"
#include "VAR_System.H"
#include "Message.H"
#include "VT100.H"
#include "EXE_Client.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void EXE_Client::notify_ignore( const String& C, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  const Group *g = vc_var.ignore( C ); if ( g == 0 ) return;

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0 ) continue;
    if ( ip == P ) continue;
    if ( ip->ignore( P ) ) continue;
    if (!ip->var.notify_ignore ) continue;
    ip->interrupt();
    ip->bell( ip->var.bell.notify_ignore() ) << Fix << P->id() << " ignore" << EOM;
    ip->ready().sync();
  }
}

void EXE_Client::notify_ignore( const SET_String& S, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) notify_ignore( *it, P, Fix );
}

// ignore
// ignore +|-|%
// ignore     <user> .. <user> | ?
// ignore +|- <user> .. <user>

void EXE_Client::ignore( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  VEC<String> list; String::parse( arg, list );

  // ignore
  if ( list.empty() ) {
    who << ": ";
    who.vt100( vt_start, "ignore" ) << ' ' << vc_var.ignore.size() << " : ";
    vc_var.ignore.print( who, who.var.admin() < admin_level, false ) << EOL;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  // ignore +|-|%
  if ( list.empty() ) {
    if ( add ) { who.var.notify_ignore = true;  who.var.modified();  return; }
    if ( del ) { who.var.notify_ignore = false; who.var.modified();  return; }
    if ( non ) {
      notify_ignore( who.var.ignore, &who, ": - " );
      vc_var. ignore.del( who.var.ignore, who.id(), true );
      who.var.ignore.erase();
      who.var.modified();
      return;
    }
  }

  if ( non ) { vc_mssg._051( who.vt100(), who ); return; }
  
  list.unique();

  // ignore + <user> .. <user>
  if ( add ) {
    SET_String Set; Set.diff( list, who.var.ignore );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( who.id() == *it ) continue; // silly
      if ( vc_var.var( *it ) == 0 ) { vc_mssg._031( who.vt100(), who, *it ); continue; }
      if ( who.var.ignore.size() >= max_ignore ) {
	vc_mssg._052( who.vt100(), who );
	break;
      }
      notify_ignore( *it, &who, ": + " );
      vc_var.ignore.add( *it, who.id(), true );
      who.var.ignore += *it;
      who.var.modified();
    }
    return;
  }
  
  // ignore - <user> .. <user>
  if ( del ) {
    SET_String Set; Set.cross( who.var.ignore, list );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      vc_var.ignore.del( *it, who.id(), true );
      who.var.ignore -= *it;
      who.var.modified();
    }
    notify_ignore( Set, &who, ": - " );
    return;
  }

  // ignore <user> .. <user> | ?
  if ( list.size() == 1 && list[0] == str_quest ) vc_var.ignore.vec( list );
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    const Group* gp = vc_var.ignore( *it );
    if ( gp == 0 ) { vc_mssg._055( who.vt100(), os, *it ); continue; }
    who << ": ";
    who.vt100( vt_start, "ignore" ) << ' ' << *gp << EOL;
  }
  who.text( os );
  return;
}


//: EXE_Client_ignore.C (eof) (c) Igor
