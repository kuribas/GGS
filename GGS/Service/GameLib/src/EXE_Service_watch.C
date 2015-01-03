// $Id: EXE_Service_watch.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service_watch.C (bof) (c) Igor Durdanovic

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

void EXE_Service::notify_watch( const String& C, const Client* P, ccptr Fix, Client* WP )
{
  TSTAT;
  
  if ( WP == 0 ) return;
  if ( WP == P ) return;
  if ( WP->ignore( P ) ) return;
  if (!WP->var.notify_watch ) return;
  WP->tell().bell( WP->var.bell.notify_watch() ) << Fix << P->id() <<" watch "<< C << EOM;
}

void EXE_Service::notify_others_watch( const String& C, const Client* P, ccptr Fix )
{
  TSTAT;

  // notify both players
  if ( P->var.play( C ) == 0 ) { // skip own game
    VC_Match *M = vc_var.matches( C );
    if ( M != 0 ) {
      notify_watch( C, P, Fix, M->req.p1 );
      if ( M->req.p2 != M->req.p1 ) notify_watch( C, P, Fix, M->req.p2 );
    }
  }

  // notify all observers
  const Group *g = vc_var.watch( C ); if ( g == 0 ) return;

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    Client* ip = vc_var.client( *it, false, false ); // no ghosts
    notify_watch( C, P, Fix, ip ); // ip gets "+ P watch C"
  }
}

void EXE_Service::notify_player_watch( const String& C, Client* P, ccptr Fix )
{
  TSTAT;

  const Group *g = vc_var.watch( C ); if ( g == 0 ) return;

  // now notify player P about observers
  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    Client* ip = vc_var.client( *it, false, false ); // no ghosts
    notify_watch( C, ip, Fix, P ); // P gets "+ ip watch C"
  }
}

void EXE_Service::notify_others_watch( const SET_String& S, const Client* P, ccptr Fix )
{
  TSTAT;
  
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) notify_others_watch( *it, P, Fix );
}

void EXE_Service::notify_player_watch( const SET_String& S, Client* P, ccptr Fix )
{
  TSTAT;
  
  SET_String::const_iterator it = S.begin();
  SET_String::const_iterator hi = S.end();
  for ( ; it != hi; ++it ) notify_player_watch( *it, P, Fix );
}

// watch
// watch +|-|%
// watch     <.id> .. <.id> | ?
// watch +|- <.id> .. <.id>

void EXE_Service::watch( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  VEC<String> list; String::parse( arg, list ); // .match.game -> .match
  for ( VEC<String>::iterator it = list.begin(), hi = list.end(); it != hi; ++it ) {
    String::size_type pos = (*it).find( '.', 1 );
    if ( pos != String::npos ) (*it).erase( pos, (*it).size() - pos );
  }

  // watch
  if ( list.empty() ) {
    P->tell().vt100( vt_start, "watch" )
      << ' ' << vc_var.watch.size() << " : " << vc_var.watch << EOM;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;
  bool non = list[0] == str_pct;

  if ( add || del || non ) list.erase( list.begin() );

  // watch +|-|%
  if ( list.empty() ) {
    if ( add ) { P->var.notify_watch = true;  P->var.modified(); return; }
    if ( del ) { P->var.notify_watch = false; P->var.modified(); return; }
    if ( non ) {
      vc_var. watch.del( P->var.watch, P->id(), false );
      P->var.watch.erase();
      P->var.modified();
      return;
    }
  }
  if ( non ) { vc_mssg._057( P->vt100(), P->tell() ) << EOM; return; }
  list.unique();

  // watch + <.id> .. <.id>
  if ( add ) {
    SET_String All; vc_var.matches.to_set( All );
    SET_String OK;  OK.cross( All, list );
    SET_String Set; Set.diff( OK, P->var.watch );
    notify_others_watch( Set, P, " + " );
    notify_player_watch( Set, P, " + " );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      VC_Match *M = vc_var.matches( *it );
      if ( M == 0 ) {
	vc_con << VCFL << "M(" << *it << ')' << endl; continue;
      }
      if ( P->var.play( *it ) != 0 ) continue; // silly
      vc_var.watch.add( *it, P->id(), false );
      P->var.watch += *it;
      P->var.modified();
      M->call_join( P->id() );
    }
    SET_String Err; Err.diff( list, OK );
    if (! Err.empty() ) {
      P->tell().vt100( vt_start, "watch" ) << " + ";
      P->vt100( vt_error, "ERR") << " not found: " << Err << EOM;
    }
    return;
  }
  
  // watch - <.id> .. <.id>
  if ( del ) {
    SET_String Set; Set.cross( P->var.watch, list );
    notify_others_watch( Set, P, " - " );
    SET_String::const_iterator it = Set.begin();
    SET_String::const_iterator hi = Set.end();
    for ( ; it != hi; ++it ) {
      vc_var.watch.del( *it, P->id(), false );
      P->var.watch -= *it;
      P->var.modified();
    }
    SET_String Err; Err.diff( list, Set );
    if (! Err.empty() ) {
      P->tell().vt100( vt_start, "watch" ) << " - ";
      P->vt100( vt_error, "ERR" ) << " not found: " << Err << EOM;
    }
    return;
  }
  
  // watch <.id> .. <.id> | ?
  if ( list.size() == 1 && list[0] == str_quest ) vc_var.watch.vec( list );
  SET_String All; vc_var.matches.to_set( All );
  SET_String OK;  OK.cross( All, list );
  if (! OK.empty() ) {
    P->tell().vt100( vt_start, "watch" ) << ' ' << OK.size();
    VEC<String>::const_iterator it = OK.begin();
    VEC<String>::const_iterator hi = OK.end();
    for ( ; it != hi; ++it ) {
      const Group* gp = vc_var.watch( *it );
      if ( gp == 0 ) {
	vc_log << VCFL << "match(" << *it << ")" << endl; // can not happen
	continue;
      }
      *P << EOL << *gp;
    }
    *P << EOM;
  }
  SET_String Err; Err.diff( list, OK );
  if (! Err.empty() ) {
    P->tell().vt100( vt_start, "watch" ) << ' ';
    P->vt100( vt_error, "ERR") << " not found: " << Err << EOM;
  }
}

//: EXE_Service_watch.C (eof) (c) Igor
