// $Id: EXE_Client_group.C 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client_group.C (bof) (c) Igor Durdanovic

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

void EXE_Client::notify_group( const String& G, const String& U, const IO_TCP_Client* P, ccptr Fix )
{
  TSTAT;
  
  const Group *g = vc_var.groups( G );
  if ( g == 0 ) { vc_con << VCFL << P->id() << " group(" << G << ')' << endl; return; }

  SET_String::const_iterator it = g->obj.begin();
  SET_String::const_iterator hi = g->obj.end();
  for ( ; it != hi; ++it ) {
    IO_TCP_Client* ip = vc_var.client( *it );
    if ( ip == 0 ) continue;
    if ( ip == P ) continue;
    if ( ip->ignore( P ) ) continue;
    if (!ip->var.notify_groups ) continue;
    ip->interrupt();
    ip->bell( ip->var.bell.notify_group() ) << Fix << U << ' ' << G << EOM;
    ip->ready().sync();
  }
}

// group
// group +|-
// group     <group> .. <group> | ?
// group +|- <group> .. <group> 
// group     <group> .. <group> +|- <user> .. <user>

void EXE_Client::group( IO_TCP_Client& who, const String& arg )
{
  TSTAT;
  
  VEC<String> list; String::parse( arg, list );

  // group
  if ( list.empty() ) {
    who << ": ";
    who.vt100( vt_start, "group" ) << ' ' << vc_var.groups.size() << " : ";
    who << vc_var.groups << EOL;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;

  if ( add || del ) list.erase( list.begin(), list.begin() + 1 );

  // group +|-
  if ( list.empty() ) {
    if ( add ) { who.var.notify_groups = true;  who.var.modified(); return; }
    if ( del ) { who.var.notify_groups = false; who.var.modified(); return; }
  }

  if ( !add && !del ) {
    SET_String::iterator it = find( list.begin(), list.end(), str_plus );
    if ( it != list.end() ) { // <group> .. <group> + <user> .. <user>
      if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
      VEC<String> user;
      copy( it + 1, list.end(), back_insert_iterator< VEC<String> >( user ) );
      list.erase( it, list.end() );

      list.unique();
      user.unique();
      
      VEC<String>::const_iterator uit = user.begin();
      VEC<String>::const_iterator uhi = user.end();
      for ( ; uit != uhi; ++uit ) {
	VAR_Client* vp = vc_var.var( *uit );
	if ( vp == 0 ) { vc_mssg._016( who.vt100(), who, *uit ) << EOL; continue; }
	VEC<String>::const_iterator git = list.begin();
	VEC<String>::const_iterator ghi = list.end();
	for ( ; git != ghi; ++git ) {
	  if ( vp->groups( *git ) != 0 ) continue;
	  add_group( who, *uit, *git );
	  notify_group( *git, *uit, &who, ": + " );
	}
      }
      return;
    }
    it = find( list.begin(), list.end(), str_minus );
    if ( it != list.end() ) { // <group> .. <group> - <user> .. <user>
      if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
      VEC<String> user;
      copy( it + 1, list.end(), back_insert_iterator< VEC<String> >( user ) );
      list.erase( it, list.end() );

      list.unique();
      user.unique();
      
      VEC<String>::const_iterator uit = user.begin();
      VEC<String>::const_iterator uhi = user.end();
      for ( ; uit != uhi; ++uit ) {
	VAR_Client* vp = vc_var.var( *uit );
	if ( vp == 0 ) { vc_mssg._016( who.vt100(), who, *uit ) << EOL; continue; }
	VEC<String>::const_iterator git = list.begin();
	VEC<String>::const_iterator ghi = list.end();
	for ( ; git != ghi; ++git ) {
	  if ( vp->groups( *git ) == 0 ) continue;
	  notify_group( *git, *uit, &who, ": - " );
	  del_group( who, *uit, *git );
	}
      }
      return;
    }
  }

  list.unique();

  // group + <group> .. <group>
  if ( add ) {
    if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
    VEC<String>::const_iterator it = list.begin();
    VEC<String>::const_iterator hi = list.end();
    for ( ; it != hi; ++it ) {
      if ( *it == group_admin || *it == group_client || *it == group_service ) {
	vc_mssg._029( who.vt100(), who, *it ); continue;
      }
      if ( (*it)[0] != '_' || ! VAR_System::name_ok( *it ) ) {
	vc_mssg._030( who.vt100(), who, *it ); continue;
      }
      if ( vc_var.groups( *it ) != 0 ) continue;
      vc_var.groups += Group( *it );
      vc_var.modified();
    }
    return;
  }
  
  // groups - <group> .. <group>
  if ( del ) {
    if ( who.var.admin() < root_level ) { vc_mssg._040( who.vt100(), who ); return; }
    VEC<String>::const_iterator it = list.begin();
    VEC<String>::const_iterator hi = list.end();
    for ( ; it != hi; ++it ) {
      if ( vc_var.groups( *it ) == 0 ) continue;
      vc_var.groups -= *it;
      vc_var.modified();
    }
    return;
  }

  // group <group> .. <group> | ?
  if ( list.size() == 1 && list[0] == str_quest ) vc_var.groups.vec( list );
  ostringstream os;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    const Group* gp = vc_var.groups( *it );
    if ( gp == 0 ) { vc_mssg._028( who.vt100(), os, *it ); continue; }
    who << ": ";
    who.vt100( vt_start, "group" ) << ' ' << *gp << EOL;
  }
  who.text( os );
}

//: EXE_Client_group.C (eof) (c) Igor
