// $Id: EXE_Service_notify.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service_notify.C (bof) (c) Igor Durdanovic

#include "Client.H"
#include "EXE_Service.H"
#include "VAR_Service.H"
#include "Message.H"
#include "VT100.H"
#include <algorithm>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void EXE_Service::notify_notify( const String& C, const Client* P, ccptr Fix )
{
  TSTAT;
  
  const Group *g = vc_var.notify( C ); if ( g == 0 ) return;

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    Client* ip = vc_var.client( *it, false, false );
    if ( ip == 0 ) continue;
    if ( ip == P ) continue;
    if ( ip->ignore( P ) ) continue;
    if (!ip->var.notify_notify ) continue;
    ip->tell().bell( ip->var.bell.notify_notify() ) << Fix << P->id() << " notify" << EOM;
  }
}

void EXE_Service::notify_notify( const SET_String& S, const Client* P, ccptr Fix )
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

void EXE_Service::notify( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;
  
  VEC<String> list; String::parse( arg, list );

  // notify
  if ( list.empty() ) {
    int len = vc_var.notify.size();
    int star_len = vc_var.star_notify.size();
    len += int( star_len > 0 );
    P->tell().vt100( vt_start, "notify" ) << ' ' << len << " : ";
    if ( star_len > 0 ) *P << "*(" << star_len << ") ";
    *P << vc_var.notify << EOM;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  if ( list.empty() ) {
    if ( add ) { P->var.notify_notify = true;  P->var.modified(); return; }
    if ( del ) { P->var.notify_notify = false; P->var.modified(); return; }
    if ( non ) {
      notify_notify( P->var.notify, P, " - " );
      vc_var.notify.del( P->var.notify, P->id(), true );
      vc_var.star_notify -= P->id();
      P->var.notify.erase();
      P->var.star_notify = false;
      P->var.modified();
      return;
    }
  }
  if ( non ) { vc_mssg._051( P->vt100(), P->tell() ) << EOM; return; }
  list.unique();
  
  // notify + [*] <user> .. <user>
  if ( add ) {
    ostringstream os;
    SET_String Set; Set.diff( list, P->var.notify );
    SET_String::iterator it = Set.begin();
    SET_String::iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( *it == str_star ) {
	vc_var.star_notify += P->id();
	P->var.star_notify = true;
	P->var.modified();
	continue;
      }
      if ( P->id() == *it ) continue;
      if ( vc_var.var( *it ) == 0 ) {
	vc_mssg._028( P->vt100(), os << EOL, *it ); continue;
      }
      if ( P->var.notify.size() >= max_notify ) {
	Set.erase( it, Set.end() );
	vc_mssg._052( P->vt100(), os << EOL ); break;
      }
      notify_notify( *it, P, " + " );
      vc_var.notify.add( *it, P->id(), true );
      P->var.notify += *it;
      P->var.modified();
    }
    if ( os.str().size() > 0 ) P->tell().vt100( vt_start, "notify" ).text( os ) << EOM;
    return;
  }
  
  // notify - [*] <user> .. <user>
  if ( del ) {
    SET_String Set;
    P->var.notify += str_star; // fake "*" being in the list for cross
    Set.cross( P->var.notify, list );
    P->var.notify -= str_star;
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( *it == str_star ) {
	vc_var.star_notify -= P->id();
	P->var.star_notify = false;
	P->var.modified();
	continue;
      }
      vc_var.notify.del( *it, P->id(), true );
      P->var.notify -= *it;
      P->var.modified();
    }
    notify_notify( Set, P, " - " );
    return;
  }
  
  // notify [*] <user> .. <user> | ?
  if ( list.size() == 1 && list[0] == str_quest ) {
    vc_var.notify.vec( list );
    list.insert( list.begin(), str_star );
  }
  P->tell().vt100( vt_start, "notify" );
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    if ( *it == str_star ) {
      *P << EOL
	 << setw(-8) << str_star << ' '
	 << setw(2) << vc_var.star_notify.size() << " : "
	 << vc_var.star_notify;
      continue;
    }
    const Group* gp = vc_var.notify( *it );
    if ( gp == 0 ) {
      vc_mssg._053( P->vt100(), os << EOL, *it );
      continue;
    }
    *P << EOL << *gp;
  }
  P->text( os ) << EOM;
}

//: EXE_Service_notify.C (eof) (c) Igor
