// $Id: EXE_Client_notify.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client_notify.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_FILE.H"
#include "IO_TCP_Client.H"
#include "VAR_System.H"
#include "Message.H"
#include "VT100.H"
#include "EXE_Client.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void EXE_Client::recv_notify( const SET_String& S, IO_TCP_Client& who )
{
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) {
    if ( *it == str_star ) { recv_notify( who ); return; }
    if ( vc_var.client( *it ) == 0 ) {
      who << ": - " << *it << endl;
    } else {
      who << ": + " << *it << endl;
    }
  }
}

void EXE_Client::notify_notify( const String& C, const IO_TCP_Client* P, ccptr Fix )
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
    ip->bell( ip->var.bell.notify_notify() ) << Fix << P->id() << " notify" << EOM;
    ip->ready().sync();
  }
}

void EXE_Client::notify_notify( const SET_String& S, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) notify_notify( *it, P, Fix );
}

// notify
// notify +|-|%
// notify     [*] <user> .. <user> | ?
// notify +|- [*] <user> .. <user>

void EXE_Client::notify( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  VEC<String> list; String::parse( arg, list );

  // notify
  if ( list.empty() ) {
    who << ": ";
    int len = vc_var.notify.size();
    int star_len = vc_var.star_notify.size();
    len += int( star_len > 0 );
    who.vt100( vt_start, "notify" ) << ' ' << len << " : ";
    if ( star_len > 0 ) who << "*(" << star_len << ") ";
    who << vc_var.notify << EOL;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  // notify +|-|%
  if ( list.empty() ) {
    if ( add ) { who.var.notify_notify = true;  who.var.modified(); return; }
    if ( del ) { who.var.notify_notify = false; who.var.modified(); return; }
    if ( non ) {
      notify_notify( who.var.notify, &who, ": - " );
      vc_var.notify.del( who.var.notify, who.id(), true );
      vc_var.star_notify -= who.id();
      who.var.notify.erase();
      who.var.star_notify = false;
      who.var.modified();
      return;
    }
  }

  if ( non ) { vc_mssg._053( who.vt100(), who ); return; }
  list.unique();

  // + [*] <user> .. <user>
  if ( add ) {
    SET_String Set; Set.diff( list, who.var.notify );
    SET_String::iterator it = Set.begin();
    SET_String::iterator hi = Set.end();
    bool skip_star = false;
    for ( ; it != hi; ++it ) {
      if ( *it == str_star ) {
	if ( who.var.star_notify ) { skip_star = true; continue; }
	vc_var.star_notify += who.id();
	who.var.star_notify = true;
	who.var.modified();
	continue;
      }
      if ( vc_var.var( *it ) == 0 ) { vc_mssg._031( who.vt100(), who, *it ); continue; }
      if ( who.id() == *it ) continue;
      if ( who.var.notify.size() >= max_notify ) {
	Set.erase( it, Set.end() );
	vc_mssg._056( who.vt100(), who ); break;
      }
      notify_notify( *it, &who, ": + " );
      vc_var.notify.add( *it, who.id(), true );
      who.var.notify += *it;
      who.var.modified();
    }
    if ( skip_star ) Set -= str_star;
    recv_notify( Set, who );
    return;
  }
  
  // - [*] <user> .. <user>
  if ( del ) {
    SET_String Set;
    who.var.notify += str_star; // we have to fake "*" being in the list to survive cross 
    Set.cross( who.var.notify, list );
    who.var.notify -= str_star; // we get it out afterwards ..
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( *it == str_star ) {
	vc_var.star_notify -= who.id();
	who.var.star_notify = false;
	who.var.modified();
	continue;
      }
      vc_var.notify.del( *it, who.id(), true );
      who.var.notify -= *it;
      who.var.modified();
    }
    notify_notify( Set, &who, ": - " );
    return;
  }

  // notify <user> .. <user> | ?
  if ( list.size() == 1 && list[0] == str_quest ) {
    vc_var.notify.vec( list );
    list.insert( list.begin(), str_star );
  }
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    if ( *it == str_star ) {
      who << ": ";
      who.vt100( vt_start, "notify" ) << ' ';
      who << setw(-8) << str_star << ' '
	  << setw(2) << vc_var.star_notify.size() << " : "
	  << vc_var.star_notify << EOL;
      continue;
    }
    const Group* gp = vc_var.notify( *it );
    if ( gp == 0 ) { vc_mssg._054( who.vt100(), os, *it ); continue; }
    who << ": ";
    who.vt100( vt_start, "notify" ) << ' ' << *gp << EOL;
  }
  who.text( os );
  return;
}

//: EXE_Client_notify.C (eof) (c) Igor
