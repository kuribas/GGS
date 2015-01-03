// $Id: IO_Server.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_Server.C (bof) (c) Igor Durdanovic

#include <iomanip>

#include "System.H"
#include "Signal.H"
#include "Actors.H"
#include "IO_Server.H"
#include "TIME_Server.H"
#include "IO_FILE.H"
#if defined(SERVER)
#include "SET_PTR_Mux.H"
#endif
#if defined(MULTIPLEXER)
#include "IO_TCP_Mux.H"
#endif

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

IO_Server::IO_Server()
{
  FD_ZERO( &fd_send );
  FD_ZERO( &fd_recv );
  FD_ZERO( &fd_excp );

  struct rlimit rlim;

  System::rlimit_get( RLIMIT_CORE, rlim ); rlim.rlim_cur = rlim.rlim_max;
  System::rlimit_set( RLIMIT_CORE, rlim ); // dump core on error
  System::rlimit_get( RLIMIT_CORE, rlim ); // so we can trace the bug

  System::rlimit_get( RLIMIT_NOFILE, rlim ); rlim.rlim_cur = rlim.rlim_max;
  System::rlimit_set( RLIMIT_NOFILE, rlim );
  System::rlimit_get( RLIMIT_NOFILE, rlim );

  vec.reserve( rlim.rlim_cur );

  for ( sint4 i = rlim.rlim_cur; --i >= 0; ) vec.push_back( 0 );

  
  vc_con << VCTIME << "  IO_Server( " << vec.size() << " )" << endl;
  if ( this == &vc_ios ) vc_ios_ready = true;
}

IO_Server::~IO_Server()
{
  if ( this == &vc_ios && ! vc_ios_ready ) return;

  vc_con << VCTIME << " ~IO_Server( " << vec.size() << " )" << endl;

  down();
  vec.erase();
}

void IO_Server::down()
{
  vector<IO*>::reverse_iterator it = vec.rbegin();
  vector<IO*>::reverse_iterator hi = vec.rend();
  for ( ; it != hi; ++it ) free_client( *it, false );
}

void IO_Server::free_mux_clients( IO_TCP_Mux* Mux )
{
  TSTAT;
  
  vector<IO*>::reverse_iterator it = vec.rbegin();
  vector<IO*>::reverse_iterator hi = vec.rend();
  for ( ; it != hi; ++it ) {
    if ( *it == 0 ) continue;
    if ( (*it)->io_mux() != Mux ) continue;
    free_client( *it, false );
  }
}

void IO_Server::sync()
{
  TSTAT;
  
#ifdef SERVER
    vc_mux.sync();
#endif
#ifdef MULTIPLEXER
    if ( vc_mux != 0 ) vc_mux->sync();
#endif
}

void IO_Server::full()
{
  TSTAT;
  
  vector<IO*>::iterator it = vec.begin();
  vector<IO*>::iterator hi = vec.end();
  for ( ; it != hi; ++ it ) {
    if ( *it == 0 ) continue;
    if ( (*it)->send_buff.full( true ) ) {
      if ( (*it)->io_mark() ) free_client( *it, true );
      else (*it)->io_mark(true); // mark it for killing
    } else (*it)->io_mark(false);
  }
}

void IO_Server::step( sint8 usec )
{
  TSTAT;
  
  sync();
    
  fd_set fd_s = fd_send;
  fd_set fd_r = fd_recv;
  fd_set fd_e = fd_excp;

  struct timeval tv;
  struct timeval* tvp;

  if ( usec < 0 ) {
    tvp = 0;
  } else {
    tvp = &tv;
    tv.tv_sec  = usec / 1000000;
    tv.tv_usec = usec % 1000000;
  }
  // either some i/o or an interrupt will cause select to return
  sint4 no = System::select_desc( vec.size(), fd_r, fd_s, fd_e, tvp );

  if ( no > 0 ) {
    vc_sig.block();

    oper( fd_r, &IO::recv );
    oper( fd_e, &IO::excp );
    oper( fd_s, &IO::send );

    vc_sig.unblock();
  }

  full();
    
  if ( vc_sig.pipe() ) { }  // empty?
  if ( vc_sig.alrm() ) { }  // empty?
}

void IO_Server::loop()
{
  TSTAT;
  
  for ( ;; ) {
#ifdef NEW_METHOD		
		step( vc_time.next() );
		vc_time.time();
#else		
    sync();
    
    fd_set fd_s = fd_send;
    fd_set fd_r = fd_recv;
    fd_set fd_e = fd_excp;

    // either some i/o or an interrupt will cause select to return
    sint4 no = System::select_desc( vec.size(), fd_r, fd_s, fd_e, 0 );

    if ( no > 0 ) {
      vc_sig.block();

      oper( fd_r, &IO::recv );
      oper( fd_e, &IO::excp );
      oper( fd_s, &IO::send );

      vc_sig.unblock();
    }

    full();
    
    if ( vc_sig.pipe() ) ;
    if ( vc_sig.alrm() ) vc_time.time();
#endif		
    if ( vc_sig.exit() ) break;
  }
}

void IO_Server::free_client( IO* Client, bool Error )
{
  TSTAT;
  
  if ( Client == 0 ) return;

  del_client( *Client );

  if ( Client->io_alloc() ) delete Client; else Client->close( Error );
}

void IO_Server::oper( fd_set& fd_tmp, IO::OPER oper )
{
  TSTAT;
  
  vector<IO*>::iterator it = vec.begin();
  vector<IO*>::iterator hi = vec.end();

  for ( ; it != hi; ++ it )
    if ( (*it) != 0 && FD_ISSET( (*it)->io_desc(), &fd_tmp ) ) {
      if (! ((*it)->*oper)() ) if (! (*it)->clog() ) free_client( *it, true );
    }
}

void IO_Server::add_client( IO& Client )
{
  TSTAT;
  
  vec[ Client.io_desc() ] = &Client;
  ++len;
}

void IO_Server::del_client( IO& Client )
{
  TSTAT;
  
  del_send( Client );
  del_recv( Client );
  del_excp( Client );
  vec[ Client.io_desc() ] = 0;
  --len;
}

void IO_Server::add_send  ( IO& Client ) { FD_SET( Client.io_desc(), &fd_send ); }
void IO_Server::add_recv  ( IO& Client ) { FD_SET( Client.io_desc(), &fd_recv ); }
void IO_Server::add_excp  ( IO& Client ) { FD_SET( Client.io_desc(), &fd_excp ); }
void IO_Server::del_send  ( IO& Client ) { FD_CLR( Client.io_desc(), &fd_send ); }
void IO_Server::del_recv  ( IO& Client ) { FD_CLR( Client.io_desc(), &fd_recv ); }
void IO_Server::del_excp  ( IO& Client ) { FD_CLR( Client.io_desc(), &fd_excp ); }

//: IO_Server.C (eof) (c) Igor
