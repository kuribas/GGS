// $Id: IO_TCP_Client.C 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Client.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "Stat.H"
#include "Histogram_HDMY.H"
#include "IO_FILE.H"
#include "IO_Server.H"
#include "EXE_Client.H"
#include "VAR_System.H"
#include "Message.H"
#include "IO_TCP_Mux.H"
#include "IO_TCP_Client.H"
#include "VT100.H"
#include "TSTAT_Client.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const uint4 user_send_size = 1 << 11; //   2K
const uint4 user_recv_size = 1 << 10; //   1K
const uint4 serv_send_size = 1 << 12; //   4K
const uint4 serv_recv_size = 1 << 13; //   8K

const uint4 user_send_buff = user_send_size * 4;  //  8K
const uint4 user_recv_buff = user_recv_size * 2;  //  2K
const uint4 serv_send_buff = serv_send_size * 16; // 64K
const uint4 serv_recv_buff = serv_recv_size * 64; //  1M

using namespace std;

IO_TCP_Client::IO_TCP_Client( sint4 Sock, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m ) 
  : IO_TCP( false, Sock, Mux, Full, m ) 
{
  TSTAT;
  
  if ( mux == 0 || ! full ) {
    sint4 val;
    System::setsockopt( desc, SOL_SOCKET, SO_KEEPALIVE, &(val = 1), sizeof(val) );
  }
  state = wait_login;
  te_create( 0, time_logon );
  ++vc_stat.n_user;
  vc_var.histo_users.update( vc_stat.n_user.current() );
  vc_var.modified();
  mssg( "msg/connect" );
  vc_mssg._001( false, *this );
  sync();

  Form( vc_log << VCTIME, "  IO_TCP_Client( %3d %3d %d %s )",
	desc, mux ? mux->io_desc() : -1, full, send_buff.method_str() ) << endl;
}

IO_TCP_Client::~IO_TCP_Client()
{
  TSTAT;
  
  te_cancel();
  --vc_stat.n_user;
  vc_var.histo_users.update( vc_stat.n_user.current() );
  vc_var.modified();

  static String rc( ".log_off" );
  if (! this->var.alias.def( rc ).empty() ) vc_exe( *this, rc );

  Form( vc_log << VCTIME, " ~IO_TCP_Client( %3d %3d %d %s )",
	desc, mux ? mux->io_desc() : -1, full, send_buff.method_str() ) << endl;

  if ( mux && full ) mux->remove( desc );

  close( false );
}

void IO_TCP_Client::close( bool Err )
{
  TSTAT;
  
  if ( desc < 0 ) return;

  if ( state == on_line ) {
    vc_exe.send_notify( *this, false );
    vc_var.del( this );
  }

  IO_TCP::close( Err );
}

void IO_TCP_Client::te_handle( sint4 Mssg, uint4 /*Time*/ )
{
  TSTAT;
  
  switch ( Mssg ) {
  case 0 : // kill event
    if ( mux == 0 || ! full ) {
      vc_ios.free_client( this, false );
    } else {
      if ( io_alloc() ) delete this; else close( true );
    }
    break;
  case 1 : // repeat event
    String _rcmd = var.vars.def( var_rcmd );
    String _rsec = var.vars.def( var_rsec );
    if (! _rcmd.empty() ) {
      vc_exe( *this, _rcmd );
      sint4 sec = atoi( _rsec.c_str() );
      if ( sec > 0 ) te_create( 1, sec );
    }
    break;
  }
}

ostream& IO_TCP_Client::te_print( ostream& os ) const
{
  TSTAT;
  
  Form( os, "IO_TCP_Client( %s %3d %3d %d %s )",
	var.login.c_str(), desc, mux ? mux->io_desc() : -1, full, send_buff.method_str() );
  return os;
}

IO_TCP_Client& IO_TCP_Client::bell( bool On )
{
  TSTAT;
  
  if ( On ) *this << vt_bell;
  return *this;
}

IO_TCP_Client& IO_TCP_Client::ready()
{
  TSTAT;
  
  const String& ready = var.vars.def( var_ready );
  const String& alert = var.vars.def( var_alert );
  
  (*this) <<
    ( srv_ready
      ? ( ready.empty() ? str_ready : ready )
      : ( alert.empty() ? str_alert : alert ) );

  IO::sync();
  
  return *this;
}

IO_TCP_Client& IO_TCP_Client::interrupt() 
{
  TSTAT;
  
  if ( var.groups( group_service ) == 0 ) {// no need to interrupt service
    if ( var.vt100 )
      *this << vt_interrupt;
    else
      *this << EOL;
  }
  return *this;
}

IO_TCP_Client& IO_TCP_Client::vt100( const String& prefix, ccptr mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  (*this) << mssg;
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

IO_TCP_Client& IO_TCP_Client::vt100( const String& prefix, const String& mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  (*this) << mssg;
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

IO_TCP_Client& IO_TCP_Client::vt100( const String& prefix, ostringstream& mssg )
{
  TSTAT;
  
  if ( vt100() ) (*this) << prefix;
  text( mssg );
  if ( vt100() ) (*this) << vt_reset;
  return *this;
}

bool IO_TCP_Client::ignore( const IO_TCP_Client* whom ) const
{
  TSTAT;
  
  if ( whom == 0 ) return false;
  return ( ( var.ignore( whom->var.login ) != 0 ) && // is in ignore list and
	   ( whom->var.admin() <= var.admin() ) )    // has lower/equal authority
    ||   ( ( var.hear == false &&                    // you don't want to hear
	     whom->var.admin() == unreg_level ) );   // unregistered users
}

bool IO_TCP_Client::work( IO_Buffer& B )
{
  TSTAT;
  
  var.last = System::clock();

  for ( ;; ) {
    char* p = B.find( '\n' ); if (! p ) break;
    sint4 n = p - B.buff(false);
    sint4 m = n; 
    if ( n > 0 && B.buff(false)[n-1] == '\r' ) m--;
    String data( B.buff(false), m ); 
    B.remove( false, n+1 );

    const char   pat( '\\'  );
    const String rep( "\r\n" );
    data.replace( pat, rep );

    switch ( state ) {

    case wait_login: {
      if (! vc_var.login_ok( data ) ) {
	vc_mssg._002( false, *this );
	vc_mssg._001( false, *this );
	sync();
	return true;
      } 
      var.load( data );
      if ( var.email.empty() ) {
	String pw;
	var.make( data, pw, str_empty, str_empty );
      }
      var.vars = Alias( "_me", data );
      vc_mssg._004( false, *this );
      state = wait_passwd;
      sync();
    } break;

    case wait_passwd: {
      if ( var.passw.size() > 0 && !(var.passw == data) ) {
	vc_mssg._005( false, *this );
	vc_mssg._001( false, *this );
	sync();
	state = wait_login;
	return true;
      }
      var.passw = data;
      te_cancel();
      IO_TCP_Client* C = vc_var.client( var.login );
      if ( C != 0 ) {                      // somebody is already logged in
	if ( var.passw != C->var.passw ) { // passw doesn't match
	  vc_mssg._005( false, *this );
	  vc_mssg._001( false, *this );
	  sync();
	  state = wait_login;
	  return true;
	} else {                           // kill old connection
	  C->clock = 0;                    // by faking a time event!
	  C->te_handle( 0, 0 );
	}
      }
      state = on_line;
      vc_var.add( this );
      ready();
      sync();
      vc_exe.send_notify( *this, true );
      vc_exe.recv_notify( *this );
      adjust();

      static String rc( ".log_on" );
      if (! this->var.alias.def( rc ).empty() ) vc_exe( *this, rc );

      if ( var.verbose.faq()  ) { vc_exe.help( *this, "faq"  ); ready(); }
      if ( var.verbose.help() ) { vc_exe.help( *this, "help" ); ready(); }
      if ( var.verbose.news() ) { vc_exe.help( *this, "news" ); ready(); }
      sync();
    } break;
    case on_line: {
      vc_exe( *this, data );
    } break;

    default : {
      vc_log << VCFL << "state(" << int(state) << ")" << endl;
      return false;
    }
    }
  }
  return true;
}

void IO_TCP_Client::adjust()
{
  TSTAT;
  
  if ( var.email.empty() ) return; // unregistered users have minimal buffers

  if ( var.groups( group_service ) == 0 ) { // clients have larger buffers
    send_buff.max = user_send_buff;
    recv_buff.max = user_recv_buff;
    excp_buff.max = user_recv_buff;
    send_size = user_send_size;
    recv_size = user_recv_size;
  } else { // services have huge buffers
    send_buff.max = serv_send_buff;
    recv_buff.max = serv_recv_buff;
    excp_buff.max = serv_recv_buff;
    send_size = serv_send_size;
    recv_size = serv_recv_size;
  }
#if 0
  if ( mux == 0 || ! full ) {
    sint4 val;
    System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF, &(val = send_size), sizeof(val) );
    System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF, &(val = recv_size), sizeof(val) );
  }
#endif  
}

//: IO_TCP_Client.C (eof) (c) Igor
