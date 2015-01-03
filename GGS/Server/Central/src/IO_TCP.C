// $Id: IO_TCP.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_TCP.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "String.H"
#include "Stat.H"
#include "IO_Server.H"
#include "IO_FILE.H"
#include <cstring>
#ifdef SERVER
#include "IO_TCP_Mux.H"
#endif
#include "IO_TCP.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const uint4 dflt_buff_size = 1 << 10; //   1K
const uint4 dflt_send_size = 1 <<  9; // 0.5K
const uint4 dflt_recv_size = 1 <<  9; // 0.5K

using namespace std;

IO_TCP::IO_TCP( bool Server, sint4 Desc, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m ) 
  : IO( Desc, Mux, Full, dflt_buff_size, dflt_buff_size, dflt_buff_size, false, true, m ),
    server( Server ),
    send_size( dflt_send_size ),
    recv_size( dflt_recv_size )
{
  TSTAT;
  
  if ( mux == 0 || ! full ) {
    sint4 flag = System::fcntl( desc, F_GETFL ) | O_NONBLOCK;
    System::fcntl( desc, F_SETFL, flag );

    sint4 val;
    System::setsockopt( desc, SOL_SOCKET, SO_DONTROUTE, &(val = 0), sizeof(val) );
    struct linger ling = { 0, 0 };
    System::setsockopt( desc, SOL_SOCKET, SO_LINGER,    &ling,      sizeof(ling) );
#if 0    
    System::setsockopt( desc, SOL_SOCKET, SO_SNDBUF,    &(val = send_size), sizeof(val) );
    System::setsockopt( desc, SOL_SOCKET, SO_RCVBUF,    &(val = recv_size), sizeof(val) );
#endif    
  }

  if ( mux == 0 || ! full ) {
    vc_ios.add_client( *this );
    vc_ios.add_recv  ( *this );
    vc_ios.add_excp  ( *this );
  }
}

IO_TCP::~IO_TCP()
{
  TSTAT;
  
  close( false );
}

void IO_TCP::close( bool Err )
{
  TSTAT;
  
  if ( desc < 0 ) return;

  if ( mux == 0 || ! full ) {
    if ( ! server ) System::sock_shutdown( desc, 2 );
    System::sock_close( desc ); 
  }
  
  if ( Err ) {
    vc_con << VCFL << "close( " << desc << " )" << endl;
  } else {
    if ( mux == 0 || ! full ) {
      if (! io_sync() ) vc_ios.del_client( *this );
    }
  }

  desc = -1;
  mux  = 0;
}

bool IO_TCP::send()
{
  TSTAT;
  
  send_buff.compress();

  return lo_send( send_buff, 0 );
}

bool IO_TCP::recv()
{
  TSTAT;
  
  if (! lo_recv( recv_buff, 0 ) ) return false;

  recv_buff.decompress();

  return work( recv_buff );
}

bool IO_TCP::excp()
{
  TSTAT;
  
  if (! lo_recv( excp_buff, MSG_OOB ) ) return false;

  excp_buff.decompress();

  return work( excp_buff );
}

bool IO_TCP::lo_send( IO_Buffer& B, uint4 Flag )
{
  TSTAT;
  
  if ( desc < 0 ) { vc_con << VCFL; return false; } // should never happen ..

  for ( bool first = true ; B.size(true) ; first = false ) {
    ccptr buff = B.buff(true);
    sint4 size = B.size(true);

    errno = 0;

    sint4 no = ::send( desc, buff, size, Flag );

    if ( no == 0 && first ) return false;
    if ( no <= 0 ) {
      if ( errno == 0            ) return true; // no error?
      if ( errno == EAGAIN       ) return true;
      if ( errno == EINTR        ) return true;
      if ( errno == EPIPE        ) return false;
      if ( errno == ECONNRESET   ) return false;
      if ( errno == EHOSTUNREACH ) return false;
#ifdef LOG_ERR
      vc_log << VCFL;
      Form( vc_log, "lo_send( %d, %p, %d ) = %d", desc, buff, size, no ) << endl << VCER;
#endif
      return false;
    }
    B.remove( true, no ); vc_stat.io_out += no;
  }
  if (! io_sync() && B.size(true) == 0 ) vc_ios.del_send( *this );

  return true;
}

bool IO_TCP::lo_recv( IO_Buffer& B, uint4 Flag )
{
  TSTAT;
  
  if ( desc < 0 ) { vc_con << VCFL; return false; } // should never happen ...

  for ( bool first = true ;; first = false ) {
    B.extend( true, recv_size );

    cptr buff = B.last(true);

    errno = 0; 

    sint4 no = ::recv( desc, buff, recv_size, Flag );

    if ( no == 0 && first ) return false;
    if ( no <= 0 ) {
      if ( errno == 0            ) return true; // no error?
      if ( errno == EAGAIN       ) return true;
      if ( errno == EINTR        ) return true;
      if ( errno == EPIPE        ) return false;
      if ( errno == ECONNRESET   ) return false;
      if ( errno == EHOSTUNREACH ) return false;
      if ( errno == ETIMEDOUT    ) return false;
#ifdef LOG_ERR
      vc_log << VCFL;
      Form( vc_log, "lo_recv( %d, %p, %d, %d ) = %d", desc, buff, recv_size, Flag, no );
      vc_log << endl << VCER;
#endif
      return false;
    }
    B.update( true, no ); vc_stat.io_inp += no;

    if ( B.full( true ) ) return true; // postpone further reading
  }
}

//: IO_TCP.C (eof) (c) Igor
