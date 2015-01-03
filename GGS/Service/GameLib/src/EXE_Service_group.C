// $Id: EXE_Service_group.C 160 2007-06-22 15:21:10Z mburo $
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
#include "Client.H"
#include "Message.H"
#include "VT100.H"
#include "VAR_Service.H"
#include "EXE_Service.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

bool EXE_Service::add_group( const String& user, const String& group )
{
  TSTAT;
  
  Group* gp = vc_var.groups( group ); if ( gp == 0 ) return false;

  gp->obj += user;

  vc_var.modified();

  return true;
}

bool EXE_Service::del_group( const String& user, const String& group )
{
  TSTAT;
  
  Group* gp = vc_var.groups( group ); if ( gp == 0 ) return false;

  gp->obj -= user;

  vc_var.modified();

  return true;
}

// group
// group     <group> .. <group> | ?
// group +|- <group> .. <group> 
// group     <group> .. <group> +|- <user> .. <user>

void EXE_Service::group( Client* P, const String& arg )
{
  TSTAT;
  
  if ( P == 0 ) return;

  if ( P->admin() < admin_level ) {
    vc_mssg._013( P->vt100(), P->tell() ) << EOM;
    return;
  }

  VEC<String> list; String::parse( arg, list );

  // group
  if ( list.empty() ) {
    P->tell();
    P->vt100( vt_start, "group" )
      << ' ' << vc_var.groups.size() << " : "
      << vc_var.groups << EOM;
    return;
  }

  bool add = list[0] == str_plus;
  bool del = list[0] == str_minus;

  if ( add || del ) list.erase( list.begin(), list.begin() + 1 );

  // group +|-
  if ( list.empty() ) {
    vc_mssg._063( P->vt100(), P->tell() ) << EOM;
    return;
  }

  if ( !add && !del ) {
    SET_String::iterator it = find( list.begin(), list.end(), str_plus );
    if ( it != list.end() ) { // <group> .. <group> + <user> .. <user>
      if ( P->admin() < admin_level ) { vc_mssg._013( P->vt100(), P->tell() ) << EOM; return; }
      VEC<String> user;
      copy( it + 1, list.end(), back_insert_iterator< VEC<String> >( user ) );
      list.erase( it, list.end() );

      list.unique();
      user.unique();
      
      VEC<String>::const_iterator uit = user.begin();
      VEC<String>::const_iterator uhi = user.end();
      for ( ; uit != uhi; ++uit ) {
	VAR_Client* vp = vc_var.var( *uit );
	if ( vp == 0 || vp->admin < user_level ) { vc_mssg._016( P->vt100(), P->tell(), *uit ) << EOM; continue; }
	VEC<String>::const_iterator git = list.begin();
	VEC<String>::const_iterator ghi = list.end();
	for ( ; git != ghi; ++git ) add_group( *uit, *git );
      }
      return;
    }
    it = find( list.begin(), list.end(), str_minus );
    if ( it != list.end() ) { // <group> .. <group> - <user> .. <user>
      if ( P->admin() < admin_level ) { vc_mssg._013( P->vt100(), P->tell() ) << EOM; return; }
      VEC<String> user;
      copy( it + 1, list.end(), back_insert_iterator< VEC<String> >( user ) );
      list.erase( it, list.end() );

      list.unique();
      user.unique();
      
      VEC<String>::const_iterator uit = user.begin();
      VEC<String>::const_iterator uhi = user.end();
      for ( ; uit != uhi; ++uit ) {
	VAR_Client* vp = vc_var.var( *uit );
	if ( vp == 0 || vp->admin < user_level ) { vc_mssg._016( P->vt100(), P->tell(), *uit ) << EOM; continue; }
	VEC<String>::const_iterator git = list.begin();
	VEC<String>::const_iterator ghi = list.end();
	for ( ; git != ghi; ++git ) del_group( *uit, *git );
      }
      return;
    }
  }

  list.unique();

  // group + <group> .. <group>
  if ( add ) {
    if ( P->admin() < root_level ) { vc_mssg._060( P->vt100(), P->tell() ) << EOM; return; }
    VEC<String>::const_iterator it = list.begin();
    VEC<String>::const_iterator hi = list.end();
    for ( ; it != hi; ++it ) {
      if ( (*it)[0] != '_' || ! VAR_Service::name_ok( *it ) ) {
	vc_mssg._058( P->vt100(), P->tell(), *it ) << EOM; continue;
      }
      if ( vc_var.groups( *it ) != 0 ) continue;
      vc_var.groups += Group( *it );
      vc_var.modified();
    }
    return;
  }
  
  // groups - <group> .. <group>
  if ( del ) {
    if ( P->admin() < root_level ) { vc_mssg._060( P->vt100(), P->tell() ) << EOM; return; }
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
  ostringstream os_err;
  bool first_grp = true;
  bool first_err = true;
  VEC<String>::const_iterator it = list.begin();
  VEC<String>::const_iterator hi = list.end();
  for ( ; it != hi; ++it ) {
    const Group* gp = vc_var.groups( *it );
    if ( gp == 0 ) {
      if (! first_err ) os_err << EOL; first_err = false;
      vc_mssg._064( P->vt100(), os_err, *it );
      continue;
    }
    if ( first_grp ) P->tell(); else *P << EOL; first_grp = false;
    P->vt100( vt_start, "group" ) << ' ' << *gp;
  }
  if ( first_grp ) {
    if ( first_err ) return;
    else { P->tell(); }
  }
  P->text( os_err ) << EOM;
}

//: EXE_Client_group.C (eof) (c) Igor
