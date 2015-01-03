// $Id: IO_FILE.C 9037 2010-07-06 04:05:44Z mburo $
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
//: IO_FILE.C (bof) (c) Igor Durdanovic

#include "Actors.H"
#include "System.H"
#include "Stat.H"
#include "IO_Server.H"
#include "IO_FILE.H"
#include <iomanip>
#include <cstdio>
#include <cstring>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

const uint4 file_buff_len = 1 << 16; // 64K

using namespace std;

IO_FILE::IO_FILE( sint4 Desc, bool Sync, bool Alloc ) 
  : IO( Desc, 0, 0, file_buff_len, 0, 0, Sync, Alloc, IO_Buffer::NONE ),
    path("*none*"), time(-1)
{
  if (! io_sync() ) vc_ios.add_client( *this );

  ready();

  Form( vc_con << VCTIME, "  IO_FILE( %d, %d, %d )", Desc, Sync, Alloc ) << endl;
}

IO_FILE::IO_FILE( ccptr Path, bool Sync, bool Alloc, bool Append, sint4 Time ) 
  : IO( -1, 0, false, file_buff_len, 0, 0, Sync, Alloc, IO_Buffer::NONE ),
    app(Append), path(Path), time(Time)
{
  open();
}

IO_FILE::~IO_FILE()
{
  if ( this == &vc_con && ! vc_con_ready ) return;
  if ( this == &vc_log && ! vc_log_ready ) return;
  
  if ( vc_con_ready ) {
    Form( vc_con << VCTIME, " ~IO_FILE( %d, %d, %d )", desc, direct, alloc ) << endl;
  }
  close( false );
}

void IO_FILE::open ()
{
  TSTAT;
  
  ccptr NamePtr = path;
  char  Name[4096];

  sprintf( Name, "%s.%s", path, System::ftime(System::clock()) );
  NamePtr = Name;

  int flags = O_CREAT | O_WRONLY | O_NONBLOCK | O_NOCTTY;
  if ( app ) flags = flags | O_APPEND;
  else       flags = flags | O_TRUNC;
  int perm = S_IRUSR | S_IWUSR | S_IRGRP;
  desc = System::file_open( NamePtr, flags, perm );

  if ( desc < 0 ) System::exit( -1 );

  if (! io_sync() ) vc_ios.add_client( *this );

  if ( time > 0 ) te_create( 0, time ); // schedule reopening of the file

  ready();
  
  Form( vc_con << VCTIME, "  IO_FILE( %s = %d, %d, %d )",
	NamePtr, desc, io_sync(), io_alloc() ) << endl;
}

void IO_FILE::close( bool Err )
{
  TSTAT;
  
  if ( desc < 0 ) return;

  if ( ! Err ) {
    send_buff.compress();
    System::file_write( desc, send_buff.buff(true), send_buff.size(true) ); // last write
  }

  if ( this == &vc_con ) vc_con_ready = false;
  if ( this == &vc_log ) vc_log_ready = false;

  System::file_close( desc );

  if ( Err ) { // we are in deep trouble, IO_Server called close
    if ( vc_con_ready ) {
      Form( vc_con << VCTIME, " IO_FILE::close( %d, %d, %d )", desc, direct, alloc ) << endl;
    }
  } else {     // otherwise it is a normal exit
    if (! io_sync() ) vc_ios.del_client( *this ); 
  }

  desc = -1;
}

void IO_FILE::ready() const
{
  TSTAT;
  
  if ( this == &vc_con ) vc_con_ready = true;
  if ( this == &vc_log ) vc_log_ready = true;
}

bool IO_FILE::send()
{
  TSTAT;
  
  if ( desc < 0 ) return false;

  send_buff.compress();

  while ( send_buff.size(true) ) {
    ccptr buff = send_buff.buff(true);
    sint4 size = send_buff.size(true);

    sint4 cnt = System::file_write( desc, buff, size );

    if ( cnt < 0 ) return false;

    send_buff.remove( true, cnt );

    if (! io_sync() ) break;
  }

  if (! io_sync() && send_buff.size(true) == 0 ) vc_ios.del_send( *this );

#if 0
  if ( io_sync() ) System::file_sync( desc );
#endif

  return true;
}

bool IO_FILE::clog()
{
  TSTAT;
  
  send_buff.remove( true, send_buff.size( true ) );
  return true;
}

void IO_FILE::te_handle( sint4 /*Mssg*/, uint4 Time )
{
  TSTAT;
  
  if ( time <= 0 ) {
    vc_con << VCFL << "time( " << Time << " ) != " << clock << endl << VCER; 
    Form( vc_con, "vc_con(%p) vc_log(%p) this(%p) path(%s) time(%d)",
	  &vc_con, &vc_log, this, path, time ) << endl;
    return;
  }

  close( false );
  open ();
}

ostream& IO_FILE::te_print( ostream& os ) const
{
  TSTAT;
  
  Form( os, "IO_FILE( %s = %d, %d, %d )", path, desc, io_sync(), io_alloc() );
  
  return os;
}

//: IO_FILE.C (eof) (c) Igor
