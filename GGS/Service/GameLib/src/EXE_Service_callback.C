// $Id: EXE_Service_callback.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service_callback.C (bof) (c) Igor Durdanovic

#include "Client.H"
#include "Group.H"
#include "VC_Match.H"
#include "EXE_Service.H"
#include "VAR_Service.H"
#include "IO_FILE.H"
#include "VC_SEQ.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

bool notify_match_start( const VC_Match& M, const Client* C )
{
  TSTAT;
  
  return C->var.play( M.id() ) ? C->var.bell.play() : C->var.bell.notify_match();
}

bool notify_match_update( const VC_Match& M, const Client* C )
{
  TSTAT;
  
  return C->var.play( M.id() ) ? C->var.bell.play() : C->var.bell.watch();
}

void EXE_Service::match_start  ( const VC_Match& M )
{
  TSTAT;
  
  SET_String set;
  SET_String wset;
  set += M.req.p1->id();
  set += M.req.p2->id();
  set.add( vc_var.star_notify );
  set.add( vc_var.star_track );
  { const Group* G = vc_var.notify( M.req.p1->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.notify( M.req.p2->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.track ( M.req.p1->id() ); if ( G ) { set.add( G->obj ); wset.add( G->obj ); } }
  { const Group* G = vc_var.track ( M.req.p2->id() ); if ( G ) { set.add( G->obj ); wset.add( G->obj ); } }
   set.add( vc_var.star_track );
  wset.add( vc_var.star_track );
  wset -= M.req.p1->id();
  wset -= M.req.p2->id();

  {
    const Group* gp1 = vc_var.ignore( M.req.p1->id() );
    const Group* gp2 = vc_var.ignore( M.req.p2->id() );
    if ( gp1 != 0 && gp2 != 0 ) {
      SET_String set12;
      set12.cross( gp1->obj, gp2->obj );
      set.del( set12 );
    }
  }
  
  VC_SEQ seq( set );
  seq.mssg( M, notify_match_start, &VC_Match::mssg_last );

  SET_String::const_iterator it = wset.begin();
  SET_String::const_iterator hi = wset.end();
  for ( ; it != hi; ++it ) {
    Client* P = vc_var.client( *it, false, false ); // no ghosts
    if ( P == 0 ) continue;
    vc_var.watch.add( M.id(), P->id(), false );
    P->var.watch += M.id();
    notify_others_watch( *it, P, " + " );
    notify_player_watch( *it, P, " + " );
  }
  
  match_join( M );
}

void EXE_Service::match_update_to_all ( const VC_Match& M )
{
  TSTAT;
  
  SET_String set;
  set += M.req.p1->id();
  set += M.req.p2->id();
  { const Group *G = vc_var.watch ( M.id() );         if ( G ) set.add( G->obj ); }
  VC_SEQ seq( set );
  seq.mssg( M, notify_match_update, &VC_Match::mssg_last );
}

void EXE_Service::match_update_to_name ( const VC_Match& M, const String& N )
{
  TSTAT;
  
  SET_String set;
  set += N;
  VC_SEQ seq( set );
  seq.mssg( M, notify_match_update, &VC_Match::mssg_last );
}

void EXE_Service::match_update_to_all_but_name ( const VC_Match& M, const String& N )
{
  TSTAT;
  
  SET_String set;
  set += M.req.p1->id();
  set += M.req.p2->id();
  { const Group *G = vc_var.watch ( M.id() );         if ( G ) set.add( G->obj ); }
  set -= N;
  VC_SEQ seq( set );
  seq.mssg( M, notify_match_update, &VC_Match::mssg_last );
}

void EXE_Service::match_end    ( const VC_Match& M )
{
  TSTAT;
  
  SET_String set;
  set += M.req.p1->id();
  set += M.req.p2->id();
  set.add( vc_var.star_notify );
  set.add( vc_var.star_track );
  { const Group* G = vc_var.notify( M.req.p1->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.notify( M.req.p2->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.track ( M.req.p1->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.track ( M.req.p2->id() ); if ( G ) set.add( G->obj ); }

  {
    const Group* gp1 = vc_var.ignore( M.req.p1->id() );
    const Group* gp2 = vc_var.ignore( M.req.p2->id() );
    if ( gp1 != 0 && gp2 != 0 ) {
      SET_String set12;
      set12.cross( gp1->obj, gp2->obj );
      set.del( set12 );
    }
  }

  { const Group *G = vc_var.watch ( M.id() );         if ( G ) set.add( G->obj ); }

  VC_SEQ seq( set );
  seq.mssg( M, notify_match_start, &VC_Match::mssg_last );
}

void EXE_Service::match_join   ( const VC_Match& M )
{
  TSTAT;
  
  SET_String set;
  set += M.req.p1->id();
  set += M.req.p2->id();
  set.add( vc_var.star_track );
  { const Group* G = vc_var.track( M.req.p1->id() ); if ( G ) set.add( G->obj ); }
  { const Group* G = vc_var.track( M.req.p2->id() ); if ( G ) set.add( G->obj ); }
  VC_SEQ seq( set );
  seq.call( M, &VC_Match::call_join );
}

//: EXE_Service_callback.C (eof) (c) Igor
