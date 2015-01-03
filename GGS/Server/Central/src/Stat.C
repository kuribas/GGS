// $Id: Stat.C 160 2007-06-22 15:21:10Z mburo $
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
//: Stat.C (bof) (c) Igor Durdanovic

#include "HHMMSS.H"
#include "USEC.H"
#include "COMMA.H"
#include "Actors.H"
#include "IO_FILE.H"
#include "Stat.H"
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <gdbm.h>
#include <zlib.h>
#include <bzlib.h>

#if defined(SERVER)
#include "VAR_System.H"
#endif

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

#define EXT2_FS 61267

using namespace std;

void IO_Stat::update( real8 Sec )
{
  TSTAT;
  
  tot += sum;
  cur = sint4( sum / Sec );
  sum = 0;
  if ( cur > max ) max = cur;
}

ostream& IO_Stat::print( ostream& os ) const
{
  TSTAT;
  
  os << setw(7) << COMMA(cur) << " / " << setw(14) << COMMA(max) << " / " << setw(14) << COMMA(tot + sum);
  return os;
}

//

ostream& N_Stat::print( ostream& os ) const
{
  TSTAT;
  
  os << setw(7) << COMMA(cur) << " / " << setw(14) << COMMA(max) << " / " << setw(14) << COMMA(tot);
  return os;
}

//

Stat::Stat()
  : io_time( System::real_time() ),
    io_last( System::real_time() )
{
  te_create( 0, 10 ); // schedule first sampling
  if ( this == &vc_stat ) vc_stat_ready = true;
}

Stat::~Stat()
{
  if ( this == &vc_stat && ! vc_stat_ready ) return;

  te_cancel();
}

void Stat::te_handle( sint4 /*Mssg*/, uint4 /*Time*/ )
{
  TSTAT;
  
  real8 dsec = real8( System::real_time() - io_last ) / real8(uSec);
  io_inp.update( dsec ); 
  io_out.update( dsec );
#if defined(SERVER)  
  vc_var.histo_inp_bytes.update( io_inp.current() );
  vc_var.histo_out_bytes.update( io_out.current() );
  vc_var.histo_users    .update( n_user.current() );
  vc_var.modified();
#endif  
  io_last = System::real_time();

#if defined(SERVER)
  void* warn_ptr = malloc( warn_memory );
  void* exit_ptr = warn_ptr == 0 ? malloc( exit_memory ) : 0;
  if ( warn_ptr == 0 ) {
    if (! exit_ptr ) { vc_con << VCFL; System::exit(-1); }
    if ( srv_ready ) { vc_con << VCFL; srv_ready = false; }
  } else {
    if (!srv_ready ) { vc_con << VCFL; srv_ready = true; }
  }
  if ( warn_ptr ) free( warn_ptr );
  if ( exit_ptr ) free( exit_ptr );
    
  struct statfs* fs = System::fs_stat( "." );
  if ( fs != 0 ) {
    if ( fs->f_bavail < exit_blocks||(fs->f_type==EXT2_FS && fs->f_ffree < exit_inodes )) {
      vc_con << VCFL; print( vc_con ) << endl;
      System::exit(-1);
    }
    if ( fs->f_bavail < warn_blocks||(fs->f_type==EXT2_FS && fs->f_ffree < warn_inodes )) {
      if ( srv_ready ) {
	vc_con << VCFL; print( vc_con ) << endl;
	srv_ready = false;
      }
    } else {
      if (! srv_ready ) {
	vc_con << VCFL; print( vc_con ) << endl;
	srv_ready = true;
      }
    }
  }
#endif

  te_create( 0, time_stat );
}

ostream& Stat::te_print ( ostream& os ) const
{
  TSTAT;
  
  os << "Stat()";
  return os;
}

ostream& Stat::print( ostream& os ) const
{
  TSTAT;
  
  struct statfs*  fs = System::fs_stat( "." );
  struct sysinfo* ss = System::sysinfo();
  struct utsname* us = System::uname();

#if defined(__GNUC__)
  os << "Compiled    :     " << __DATE__ << ' ' << __TIME__ " with gcc " << __VERSION__<< EOL;
#endif
  os << "Current     : " << System::dtime( System::clock() ) << EOL;
  {
    os << "On-line     : "
       << System::dtime( io_time/uSec ) << " : ";
    sint8 rt = System::real_time() - io_time;
    sint8 ut = System::user_time();
    real8 f = 0; if ( rt > 0 ) f = ut * 100.0 / rt;
    Form( os, "%7.3f = ", f );
    os << USEC( ut ) << " / " << USEC( rt ) << EOL;
  }
  os << "Rebooted    : ";
  if ( ss == 0 ) {
    os << "system error." << EOL;
  } else {
    os << System::dtime( System::clock() - ss->uptime ) << " : "
       << HHMMSS( ss->uptime ) << EOL;
  }

  os << "Connections : " << n_user << EOL;
  os << "Inp [N/M/T] : " << io_inp << EOL;
  os << "Out [N/M/T] : " << io_out << EOL;
  if ( io.Txt() != 0 ) io.print( os << "IO  [%=C/D] : ", "%7.3f = ", 14, 14 ) << EOL;
  if ( db.Txt() != 0 ) db.print( os << "DB  [%=C/D] : ", "%7.3f = ", 14, 14 ) << EOL;

  os << "FS [blocks] : ";
  if ( fs == 0 ) {
    os << "system error." << EOL;
  } else {
    real8 f = 0; if ( fs->f_blocks > 0 ) f = fs->f_bavail * 100.0 / fs->f_blocks;
    Form( os, "%7.3f = ", f );
    os << setw(14) << COMMA( fs->f_bavail ) << " / "
       << setw(14) << COMMA( fs->f_blocks ) << " x "
       << setw( 5) << COMMA( fs->f_bsize  ) << EOL;
  }
  if ( fs->f_type == EXT2_FS ) {
    os << "FS [inodes] : ";
    if ( fs == 0 ) {
      os << "system error.";
    } else {
      real8 f = 0; if ( fs->f_files > 0 ) f = fs->f_ffree * 100.0 / fs->f_files;
      Form( os, "%7.3f = ", f );
      os << setw(14) << COMMA( fs->f_ffree ) << " / "
	 << setw(14) << COMMA( fs->f_files ) << EOL;
    }
  }
  os << "RAM [%=F/T] : ";
  if ( ss == 0 ) {
    os << "system error." << EOL;
  } else {
    real8 f = 0; if ( ss->totalram > 0 ) f = ss->freeram * 100.0 / ss->totalram;
    Form( os, "%7.3f = ", f );
    os << setw(14) << COMMA( ss->freeram   ) << " / "
       << setw(14) << COMMA( ss->totalram  ) << EOL;
  }
  os << "SWAP[%=F/T] : ";
  if ( ss == 0 ) {
    os << "system error." << EOL;
  } else {
    real8 f = 0; if ( ss->totalswap > 0 ) f = ss->freeswap * 100.0 / ss->totalswap;
    Form( os, "%7.3f = ", f );
    os << setw(14) << COMMA( ss->freeswap  ) << " / "
       << setw(14) << COMMA( ss->totalswap ) << EOL;
  }
  os << "Running "
     << us->sysname << ' '
     << us->release << ' '
     << us->version << ' '
     << us->machine << EOL;
  os << gdbm_version << ", "
     << "zLib " << zlibVersion() << ", "
     << "bzLib " << BZ2_bzlibVersion();

  return os;
}

//: Stat.C (eof) (c) Igor
