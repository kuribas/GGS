// $Id: IO_TCP_Service.C 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Service.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "IO_Server.H"
#include "IO_FILE.H"
#include "Client.H"
#include "EXE_Service.H"
#include "VAR_Service.H"
#include "IO_TCP_Service.H"
#include "Signal.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const uint4 serv_send_size = 1 << 13; //   8K
const uint4 serv_recv_size = 1 << 12; //   4K
const uint4 serv_send_buff = serv_send_size * 64; //  1M
const uint4 serv_recv_buff = serv_recv_size * 16; // 64K

using namespace std;

IO_TCP_Service::IO_TCP_Service( sint4 desc, IO_Buffer::Method m ) 
  : IO_TCP( false, desc, 0, 0, m )
{ 
  send_buff.max = serv_send_buff;
  recv_buff.max = serv_recv_buff;
  excp_buff.max = serv_recv_buff;
  send_size = serv_send_size;
  recv_size = serv_recv_size;

  sint4 val;
  System::setsockopt( desc, SOL_SOCKET, SO_KEEPALIVE, &(val = 1), sizeof(val) );
#if 0  
  System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF,    &(val = send_size), sizeof(val) );
  System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF,    &(val = recv_size), sizeof(val) );
#endif
  vc_con << VCTIME << "  IO_TCP_Service( " << desc << " )" << endl;
#if LOG > 2
  vc_con << "fcntl( " << desc << ", F_GETFL ) = " << System::sock_stat(desc) << endl;
  System::txtsockopt ( desc, vc_log );
#endif

  *this << login_service << endl;
  *this << passw_service << endl;
  *this << "chann %" << endl;
  *this << "verbose -news -ack -help -faq" << endl;
  *this << "bell -t -tc -tg -n -ng -nc -ni -nn" << endl;
  
  sync();

  state = skip;
}

IO_TCP_Service::~IO_TCP_Service()
{
  vc_con << VCTIME << " ~IO_TCP_Service( " << desc << " )" << endl;
  close( false );

  vc_sig.send( SIGINT );
}

void IO_TCP_Service::close( bool Err )
{
  if ( desc < 0 ) return;

  te_cancel();

  IO_TCP::close( Err );
}

void IO_TCP_Service::te_handle( sint4 /*Mssg*/, uint4 Time )
{
  if ( clock != Time ) {
    vc_log << VCFL << "time( " << Time << " ) != " << clock << endl << VCER;
    return;
  }
  vc_ios.free_client( this, true );
}

ostream& IO_TCP_Service::te_print ( ostream& os ) const
{
  os << "IO_TCP_Service( " << desc << " )";

  return os;
}

bool IO_TCP_Service::work( IO_Buffer& B )
{
  TSTAT;
  
  for ( ;; ) {
    char* p = B.find( '\n' ); if (! p ) break;
    sint4 n = p - B.buff(false);
    sint4 m = n; 
    if ( n > 0 && B.buff(false)[n-1] == '\r' ) m--;
    String data( B.buff(false), m ); 
    B.remove( false, n+1 );

    if ( data.empty() ) continue;
    if ( vc_var.ready( data ) ) { state = ready; continue; }
    if ( state == skip ) continue;

    String who;
    String msg;
    String::parse( data, who, msg, ':' );

    Client* P = 0;
    
    if (! who.empty() ) {
      P = vc_var.client( who, false, true );
      if ( P == 0 ) {
#if 0	
	vc_log << VCFL << "work( " << who << " ) : " << msg << endl;
#endif
	return true;
      }
      if ( P->var.ghost ) vc_con << VCFL << P->id() << endl;
    }
    vc_exe( P, msg );
  }

  vc_var.sync(); // perform buffers sync & i/o
  
  return true;
}

//: IO_TCP_Service.C (eof) (c) Igor
