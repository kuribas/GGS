// $Id: EXE_Client_chann.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client_chann.C (bof) (c) Igor Durdanovic

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

void EXE_Client::notify_chann( const String& C, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  const Group *g = vc_var.channs( C ); if ( g == 0 ) return;

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0 ) continue;
    if ( ip == P ) continue;
    if ( ip->ignore( P ) ) continue;
    if (!ip->var.notify_channs ) continue;
    ip->interrupt();
    ip->bell( ip->var.bell.notify_chann() ) << Fix << P->id() << ' ' << C << EOM;
    ip->ready().sync();
  }
}

void EXE_Client::notify_chann( const SET_String& S, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) notify_chann( *it, P, Fix );
}

// chann
// chann +|-|%
// chann     <chann> .. <chann> | ?
// chann +|- <chann> .. <chann>

void EXE_Client::chann( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  VEC<String> list; String::parse( arg, list );

  // chann
  if ( list.empty() ) {
    who << ": ";
    who.vt100( vt_start, "chann" ) << ' ' << vc_var.channs.size() << " : ";
    vc_var.channs.print( who, who.var.admin() < admin_level, false ) << EOL;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  // chann +|-|%
  if ( list.empty() ) {
    if ( add ) { who.var.notify_channs = true;  who.var.modified();  return; }
    if ( del ) { who.var.notify_channs = false; who.var.modified();  return; }
    if ( non ) {
      notify_chann( who.var.channs, &who, ": - " );
      vc_var. channs.del( who.var.channs, who.id(), true );
      who.var.channs.erase();
      who.var.modified();
      return;
    }
  }
  if ( non ) { vc_mssg._050( who.vt100(), who ); return; }
  list.unique();

  // chann + <chann> .. <chann>
  if ( add ) {
    SET_String Set; Set.diff( list, who.var.channs );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( (*it)[0] != '.' || ! VAR_System::name_ok( *it ) ) {
	vc_mssg._026( who.vt100(), who, *it ); continue;
      }
      if ( who.var.channs.size() >= max_chann ) {
	vc_mssg._027( who.vt100(), who ); break;
      }
      notify_chann( *it, &who, ": + " );
      vc_var.channs.add( *it, who.id(), true );
      who.var.channs += *it;
      who.var.modified();
    }
    return;
  }
  
  // chann - <chann> .. <chann>
  if ( del ) {
    SET_String Set; Set.cross( who.var.channs, list );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      vc_var.channs.del( *it, who.id(), true );
      who.var.channs -= *it;
      who.var.modified();
    }
    notify_chann( Set, &who, ": - " );
    return;
  }

  // chann <chann> .. <chann> | ?
  if ( list.size() == 1 && list[0] == str_quest ) {
    vc_var.channs.vec( list );
    for ( sint4 i = list.size(); --i >= 0; ) // remove hidden channels
      if ( list[i][1] == '.' && who.var.channs( list[i] ) == 0 )
	list.erase( list.begin() + i );
  }
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    const Group* gp = vc_var.channs( *it );
    if ( gp == 0 ) { vc_mssg._025( who.vt100(), os, *it ); continue; }
    who << ": ";
    who.vt100( vt_start, "chann" ) << ' ' << *gp << EOL;
  }
  who.text( os );
  return;
}


//: EXE_Client_chann.C (eof) (c) Igor
