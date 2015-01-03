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
#include "VT100.H"
#include "Client.H"
#include "VAR_Service.H"
#include "EXE_Service.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void VC_SEQ::mssg( const VC_Match& M, BellFun1 BF, VC_Match::MssgFun MF )
{
  TSTAT;
  
  ostringstream os_arr[2][2]; // bell x client
  bool       os_any[2][2];
  for ( int b = 0; b < 2; ++b )
    for ( int m = 0; m < 2; ++m ) {
      os_arr[b][m] << "tell ";
      os_any[b][m] = false;
    }

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    Client* C = vc_var.client( *it, false, false ); // no ghosts
    if ( C == 0 ) continue; // user might not be here (ghost) any more!
    int b = BF( M, C ) ? 1 : 0;
    int m = C->var.client ? 1 : 0;
    if ( os_any[b][m] ) os_arr[b][m] << ',';
    os_arr[b][m] << C->id();
    os_any[b][m] = true;
  }

  if ( os_any[0][0] ) (M.*MF)( os_arr[0][0] << ' ', false );
  if ( os_any[0][1] ) (M.*MF)( os_arr[0][1] << ' ', true );
  if ( os_any[1][0] ) (M.*MF)( os_arr[1][0] << ' ' << vt_bell, false );
  if ( os_any[1][1] ) (M.*MF)( os_arr[1][1] << ' ' << vt_bell, true );
  
  if ( vc_var.connected() ) {
    for ( int b = 0; b < 2; ++b )
      for ( int m = 0; m < 2; ++m ) {
	if ( os_any[b][m] ) vc_var.socket->text( os_arr[b][m] ) << EOM;
      }
  }
}

void VC_SEQ::call( const VC_Match& M, VC_Match::CallFun CF )
{
  TSTAT;
  
  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) (M.*CF)( *it );
}

void VC_SEQ::text( const Client* from, ostringstream& os, char C, BellFun2 BF )
{
  TSTAT;
  
  ostringstream os_arr[2]; // bell
  bool       os_any[2]; // bell
  for ( uint4 i = 0; i < 2; ++i ) {
    os_arr[i] << "tell ";
    os_any[i] = false;
  }

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    Client* P = vc_var.client( *it, false, false );
    if ( P == 0    ) continue; // user might not be here (ghost) any more!
    // vc_con << VCFL << *it << endl; vc_con.text( os ) << endl; continue;
    if ( P == from ) continue;
    int b = BF( C, P );
    if ( os_any[b] ) os_arr[b] << ',';
    os_arr[b] << P->id();
    os_any[b] = true;
  }

  os_arr[0] << ' ';
  os_arr[1] << ' ' << vt_bell;
  
  if ( vc_var.connected() ) {
    for ( uint4 i = 0; i < 2; ++i )
      if ( os_any[i] ) vc_var.socket->text( os_arr[i] ).text( os ) << EOM;
  }
}

void VC_SEQ::text( const Client* from, ostringstream& os, const VC_Match& M, BellFun1 BF )
{
  TSTAT;
  
  ostringstream os_arr[2]; // bell
  bool       os_any[2]; // bell
  for ( uint4 i = 0; i < 2; ++i ) {
    os_arr[i] << "tell ";
    os_any[i] = false;
  }

  SET_String::const_iterator it = Set.begin();
  SET_String::const_iterator hi = Set.end();
  for ( ; it != hi; ++it ) {
    Client* P = vc_var.client( *it, false, false );
    if ( P == 0    ) continue; // user might not be here (ghost) any more!
    // vc_con << VCFL << *it << endl; vc_con.text( os ) << endl; continue;
    if ( P == from ) continue;
    int b = BF( M, P );
    if ( os_any[b] ) os_arr[b] << ',';
    os_arr[b] << P->id();
    os_any[b] = true;
  }

  os_arr[0] << ' ';
  os_arr[1] << ' ' << vt_bell;
  
  if ( vc_var.connected() ) {
    for ( uint4 i = 0; i < 2; ++i )
      if ( os_any[i] ) vc_var.socket->text( os_arr[i] ).text( os ) << EOM;
  }
}

//: VC_SEQ.C (eof) (c) Igor
