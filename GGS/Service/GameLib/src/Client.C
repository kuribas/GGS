// $Id: Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: Client.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "Client.H"
#include "Stat.H"
#include "IO_Buffer.H"
#include "VAR_Service.H"
#include "EXE_Service.H"
#include "SET_PTR_Request.H"
#include "SET_PTR_Match.H"
#include "VT100.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

Client::Client( bool /*Ghost*/ )
  : ostream( &send_buff ),
    send_buff( IO_Buffer::NONE, 1024, 0 ),
    dtor_(false)
{
  TSTAT;
  
  ++vc_stat.n_user;
}

Client::~Client()
{
  TSTAT;
  
  --vc_stat.n_user;
  dtor_ = true;

  clean_requests();
  clean_matches( true );
  clean_watches();
}

void Client::alive2ghost()
{
  TSTAT;
  
  dtor_ = true;

  var.ghost = true;
  
  clean_requests();
  clean_matches( false );
  clean_watches();

  dtor_ = false;
}

void Client::ghost2alive()
{
  var.ghost = false;
  vc_var.add( this );

  SET_String::const_iterator it = var.play.begin();
  SET_String::const_iterator hi = var.play.end();
  for ( ; it != hi; ++it ) { // revive matches
    VC_Match* M = vc_var.matches( *it );
    if ( M == 0 ) { // can not happen
      vc_log << VCFL << "match " << *it << "not here ?! for " << id() << endl;
      continue;
    }
    M->call_join( id() );
  }
}

void Client::ghost2void()
{
  if (! ghost() ) return;
  if ( in_tournament() ) return;

  vc_var.del( this );

  delete this;
}

void Client::clean_requests()
{
  TSTAT;
  
  vc_var.global.erase( var.send );
  vc_var. local.erase( var.send );
  vc_var. local.erase( var.recv );
  var.send.erase();
  var.recv.erase();
}

void Client::clean_matches ( bool All )
{
  TSTAT;
  
  vc_var.matches.erase( var.play, All );
}
  

void Client::clean_watches ()
{
  TSTAT;
  
  vc_var.watch.del( var.watch, id(), false );
  vc_exe.notify_others_watch( var.watch, this, " - " );
  var.watch.erase();
}
  

sint4 Client::playing( bool rand ) const
{
  sint4 cnt = 0;
  for ( SET_String::const_iterator it = var.play.begin(), hi = var.play.end(); it != hi; ++it ) {
    VC_Match* m = vc_var.matches( *it );
    if ( m == 0 ) {
      vc_con << VCFL;
      vc_con << "login: " << var.login << ", match:" << *it << " not found!? "
	     << "in matches: " << vc_var.matches.size();
      SET_PTR_Match::const_iterator it = vc_var.matches.begin();
      SET_PTR_Match::const_iterator hi = vc_var.matches.end();
      for ( ; it != hi; ++it ) (*it)->print( vc_con << ' ' );
      vc_con << endl;
      continue;
    }
    if ( rand == m->type().is_rand_game() ) ++cnt;
  }
  return cnt;
}

Client& Client::vt100( const String& prefix, ccptr mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  (*this) << mssg;
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

Client& Client::vt100( const String& prefix, const String& mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  (*this) << mssg;
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

Client& Client::vt100( const String& prefix, ostringstream& mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  text( mssg );
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

bool Client::ignore( const Client* whom ) const
{
  TSTAT;
  
  if ( whom == 0 ) return false;
  return ( ( var.ignore( whom->id() ) != 0 ) ||      // is in ignore list or
	   ( var.hear == false &&                    // you don't want to hear
	     whom->var.admin == unreg_level ) );     // unregistered users
}

bool Client::in_tournament() const
{
  SET_String::const_iterator it = var.play.begin();
  SET_String::const_iterator hi = var.play.end();
  for ( ; it != hi; ++it ) {
    const VC_Match* M = vc_var.matches( *it );
    if ( M == 0 ) { // can not happen
      vc_log << VCFL << "match " << *it << "not here ?! for " << id() << endl;
      continue;
    }
    if ( M->tid().empty() ) continue;
    return true;
  }
  return false;
}

Client& Client::text(   ostringstream& os )
{
  TSTAT;
  
  send_buff.Tappend( os );
  return *this;
}

Client& Client::mssg( const String& file )
{
  TSTAT;
  
  send_buff.Tappend( file.c_str(), false );
  return *this;
}

Client& Client::tell()
{
  TSTAT;
  
  *this << var.tell_prefix;
  return *this;
}

Client& Client::bell( bool On )
{
  TSTAT;
  
  if ( On ) *this << "";
  return *this;
}

//: Client.C (eof) (c) Igor
