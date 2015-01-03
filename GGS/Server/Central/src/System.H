// $Id: System.H 160 2007-06-22 15:21:10Z mburo $
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
//: System.H (bof) (c) Igor Durdanovic

#ifndef System_H
#define System_H

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <sstream>

#include "CPP.H"

#if defined (glibc)
# define RLIMIT_RESOURCE __rlimit_resource
# define SOCKLEN_T socklen_t
#else
# define RLIMIT_RESOURCE int
# define SOCKLEN_T int
#endif

class System
{
public:
  static sint8 real_time(); // in micro sec
  static sint8 user_time(); // in micro sec

  static sint4   clock();          // curr time in sec since 19..
  static ccptr   dtime( sint4 c ); // converts clock() into string
  static ccptr   stime( sint4 c ); // converts clock() into SHORT string
  static ccptr   ftime( sint4 c ); // converts clock() into FILE-NAME string
  static ccptr ggftime( sint4 c ); // converts clock() into GGF DT[] string

  static void free   ( vptr Ptr );
  static vptr malloc (           sint4 Len );
  static vptr realloc( vptr Ptr, sint4 Len );

  static void chroot( ccptr Path );
  static void chdir ( ccptr Path );
  static void exit  ( sint4 RetCode );

  static bool  file_legal( ccptr Path ); // no .. or //
  static bool  file_exist( ccptr Path );
  static sint4 file_size ( ccptr Path );
  static sint4 file_open ( ccptr Path, sint4 Flags, mode_t Mode );
  static void  file_sync ( sint4 Desc );
  static void  file_close( sint4 Desc );
  static sint4 file_write( sint4 Desc, cvptr Buff, sint4 Count );
  static sint4 file_read ( sint4 Desc,  vptr Buff, sint4 Count );

  static struct statfs*  fs_stat( ccptr Path );
  static struct sysinfo* sysinfo();
  static struct utsname* uname();
  
  static sint4 sock_open    ( sint4 Domain, sint4 Type, sint4 Protocol );
  static sint4 sock_connect ( ccptr Host, uint4 Port, bool Exit );
  static sint4 sock_accept  ( sint4 Sock );
  static void  sock_bind    ( sint4 Sock, sint4 Domain, uint4 Port );
  static void  sock_listen  ( sint4 Sock, sint4 OnHold );
  static void  sock_shutdown( sint4 Sock, sint4 How );
  static void  sock_close   ( sint4 Sock);

  static ccptr sock_stat( sint4 Sock );

  static void rlimit_get( RLIMIT_RESOURCE What, struct rlimit& rlim );
  static void rlimit_set( RLIMIT_RESOURCE What, struct rlimit& rlim );
  static void rusage_get( struct rusage& ruse );

  static sint4 select_desc( sint4 N, fd_set& Read, fd_set& Write, fd_set& Excp, struct timeval* T );

  static sint4 fcntl( sint4 Desc, sint4 Cmd );
  static sint4 fcntl( sint4 Desc, sint4 Cmd, long Arg );

  static sint4 getsockopt( sint4 Sock, sint4 Level, sint4 Opt, vptr Val, SOCKLEN_T* Len );
  static sint4 setsockopt( sint4 Sock, sint4 Level, sint4 Opt, vptr Val, SOCKLEN_T  Len );
  static void  txtsockopt( sint4 Sock, std::ostream& os );

  static sint4 getpeername( sint4 Sock, struct sockaddr* sa, SOCKLEN_T* Len );
  static sint4 getsockname( sint4 Sock, struct sockaddr* sa, SOCKLEN_T* Len );

  static struct hostent* gethostbyaddr( const struct sockaddr* Addr, sint4 Len, sint4 Type );
  static void            gethostname  ( String& host_name );
  static void            getsockhost  ( String& host_name, String& host_ip, sint4& Port, sint4 Sock );

private:
  static bool shutdown;
};

#endif

//: System.H (eof) (c) Igor
