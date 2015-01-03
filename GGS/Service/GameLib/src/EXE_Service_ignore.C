// $Id: EXE_Service_ignore.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service_ignore.C (bof) (c) Igor Durdanovic

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

void EXE_Service::notify_ignore( const String& C, const Client* P, ccptr Fix )
{
  TSTAT;
  
  const Group *g = vc_var.ignore( C ); if ( g == 0 ) return;

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    Client* ip = vc_var.client( *it, false, false ); // no ghosts
    if ( ip == 0 ) continue;
    if ( ip == P ) continue;
    if ( ip->ignore( P ) ) continue;
    if (!ip->var.notify_ignore ) continue;
    ip->tell().bell( ip->var.bell.notify_ignore() ) << Fix << P->id() << " ignore" << EOM;
  }
}

void EXE_Service::notify_ignore( const SET_String& S, const Client* P, ccptr Fix )
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

void EXE_Service::ignore( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list );

  // ignore
  if ( list.empty() ) {
    P->tell().vt100( vt_start, "ignore" )
      << ' ' << vc_var.ignore.size() << " : " << vc_var.ignore << EOM;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  // ignore +|-|%
  if ( list.empty() ) {
    if ( add ) { P->var.notify_ignore = true;  P->var.modified(); return; }
    if ( del ) { P->var.notify_ignore = false; P->var.modified(); return; }
    if ( non ) {
      notify_ignore( P->var.ignore, P, " - " );
      vc_var.ignore.del( P->var.ignore, P->id(), true );
      P->var.ignore.erase();
      P->var.modified();
      return;
    }
  }
  if ( non ) { vc_mssg._048( P->vt100(), P->tell() ) << EOM; return; }
  list.unique();

  // ignore + <user> .. <user>
  if ( add ) {
    ostringstream os;
    SET_String Set; Set.diff( list, P->var.ignore );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      if ( P->id() == *it ) continue;
      if ( vc_var.var( *it ) == 0 ) {
	vc_mssg._028( P->vt100(), os << EOL, *it ); continue;
      }
      if ( P->var.ignore.size() >= max_ignore ) {
	vc_mssg._049( P->vt100(), os << EOL ); break;
      }
      notify_ignore( *it, P, " + " );
      vc_var.ignore.add( *it, P->id(), true );
      P->var.ignore += *it;
      P->var.modified();
    }
    if ( os.str().size() > 0 ) P->tell().vt100( vt_start, "ignore" ).text( os ) << EOM;
    return;
  }
  
  // ignore - <user> .. <user>
  if ( del ) {
    SET_String Set; Set.cross( P->var.ignore, list );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      vc_var.ignore.del( *it, P->id(), true );
      P->var.ignore -= *it;
      P->var.modified();
    }
    notify_ignore( Set, P, " - " );
    return;
  }
  
  // ignore <user> .. <user> | ?
  if ( list.size() == 1 && list[0] == str_quest ) vc_var.ignore.vec( list );
  P->tell().vt100( vt_start, "ignore" );
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    const Group* gp = vc_var.ignore( *it );
    if ( gp == 0 ) {
      vc_mssg._050( P->vt100(), os << EOL, *it );
      continue;
    }
    *P << EOL << *gp;
  }
  P->text( os ) << EOM;
}

//: EXE_Service_ignore.C (eof) (c) Igor
