// $Id: System.C 160 2007-06-22 15:21:10Z mburo $
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
//: System.C (bof) (c) Igor Durdanovic

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include "System.H"
#include "Actors.H"
#include "String.H"
#include "IO_FILE.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

#define PEER_NAME

bool System::shutdown = false;

using namespace std;

sint8 System::real_time()
{
  struct timeval tv;

  sint4 ok = gettimeofday( &tv, 0 );

  if ( ok != 0 ) {
    vc_con << VCFL;
    Form( vc_con, "gettimeofday( %p, 0 ) = %d", &tv, ok ) << endl << VCER;
    return 0;
  }

  sint8 sec  = tv.tv_sec;
  sint8 usec = tv.tv_usec;

  return sec * uSec + usec;
}

sint8 System::user_time()
{
  struct rusage ru;

  rusage_get( ru );

  sint8 sec  = ru.ru_utime.tv_sec  + ru.ru_stime.tv_sec;
  sint8 usec = ru.ru_utime.tv_usec + ru.ru_stime.tv_usec;

  return sec * uSec + usec;
}

sint4 System::clock()
{
  TSTAT;
  
  return ::time(0);
}

ccptr System::dtime( sint4 c )
{
  TSTAT;
  
  time_t C = c;
  struct tm* tc = localtime( &C );
  static char stc[1024];
  strftime( stc, 1024, "%a %d %b %Y %H:%M:%S %Z", tc );

  return stc;
}

ccptr System::stime( sint4 c )
{
  TSTAT;
  
  time_t C = c;
  struct tm* tc = localtime( &C );
  static char stc[1024];
  strftime( stc, 1024, "%d %b %Y %H:%M:%S", tc );

  return stc;
}

ccptr System::ftime( sint4 c )
{
  TSTAT;
  
  time_t C = c;
  struct tm* tc = localtime( &C );
  static char stc[1024];
  strftime( stc, 1024, "%Y.%m.%d_%H:%M:%S", tc );

  return stc;
}

ccptr System::ggftime( sint4 c )
{
  TSTAT;
  
  time_t C = c;
  struct tm* tc = localtime( &C );
  static char stc[1024];
  strftime( stc, 1024, "%Y.%m.%d_%H:%M:%S.%Z", tc );

  return stc;
}

void System::free( vptr Ptr )
{
  TSTAT;
  
  if ( Ptr ) ::free( Ptr );
}

vptr System::malloc( sint4 Len )
{
  TSTAT;
  
  if ( Len <= 0 ) return 0;
  void *p = ::malloc( Len );
  if (! p ) {
#ifdef LOG_ERR
    vc_con << VCFL << "malloc( " << Len << " )" << endl << VCER;
#endif
    exit( errno );
  }
  return p;
}

vptr System::realloc( vptr Ptr, sint4 Len )
{
  TSTAT;
  
  if (! Ptr ) return malloc( Len );
  vptr p = ::realloc( Ptr, Len );
  if (! p ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "realloc( %p, %d )", Ptr, Len ) << endl << VCER;
#endif
    exit( errno );
  }
  return p;  
}

void System::chroot( ccptr Path )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::chroot( Path );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "chdir( " << Path << " ) = " << ok << endl << VCER;
#endif
    exit( errno );
  }  
}

void System::chdir( ccptr Path )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::chdir( Path );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "chdir( " << Path << " ) = " << ok << endl << VCER;
#endif
    exit( errno );
  }  
}

void System::exit( sint4 RetCode )
{
  TSTAT;
  
  if ( shutdown ) return;

  shutdown = true;

  // try to exit gracefully

  ::exit( RetCode );
}

//

bool System::file_legal( ccptr Path )
{
  TSTAT;
  
  for ( ; *Path ; ++Path ) {
    if ( Path[0] == '.' && Path[1] == '.' ) return false;
    if ( Path[0] == '/' && Path[1] == '/' ) return false;
  }
  return true;
}

bool System::file_exist( ccptr Path )
{
  TSTAT;
  
  struct stat stat_buf;
  sint4 ok = ::stat( Path, &stat_buf );
  return ( ok == 0 );
}

sint4 System::file_size( ccptr Path )
{
  TSTAT;
  
  errno = 0;

  struct stat stat_buf;
  sint4 ok = ::stat( Path, &stat_buf );
  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "stat( " << Path << ", ... ) = " << ok << endl << VCER;
#endif
    return 0;
  }
  return stat_buf.st_size;
}

sint4 System::file_open( ccptr Path, sint4 Flags, mode_t Mode )
{
  TSTAT;
  
  errno = 0;

  sint4 desc = ::open( Path, Flags, Mode );

  if ( desc < 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "open( " << Path << ", " << Flags << ", " << Mode << " ) = " << desc << endl << VCER;
#endif
  }

  return desc;
}

void System::file_sync( sint4 Desc )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::fsync( Desc );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "fdatasync( " << Desc << " ) = " << ok << endl << VCER;
#endif
  }
}

void System::file_close( sint4 Desc )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::close( Desc );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "close( " << Desc << " ) = " << ok << endl << VCER;
#endif
  }
}

sint4 System::file_read( sint4 Desc, vptr Buff, sint4 Count )
{
  TSTAT;
  
  errno = 0;

  sint4 cnt = ::read( Desc, Buff, Count );

  if ( cnt < 0 ) {
    if ( errno == EAGAIN ) return 0;
    if ( errno == EINTR  ) return 0;
    if ( errno == 0      ) return 0;

#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "read( %d, %p, %d ) = %d", Desc, Buff, Count, cnt ) << endl << VCER;
#endif
  }

  return cnt; // propagate the result
}

sint4 System::file_write( sint4 Desc, cvptr Buff, sint4 Count )
{
  TSTAT;
  
  errno = 0;

  sint4 cnt = ::write( Desc, Buff, Count );

  if ( cnt < 0 ) {
    if ( errno == EAGAIN ) return 0;
    if ( errno == EINTR  ) return 0;
    if ( errno == 0      ) return 0;
    if ( errno == ENOSPC ) {
      // can not happen, but just in case it happens we are lost .. we can not notify anybody
      if ( Desc == vc_con.io_desc() ) {
	exit( errno );
      }

      // might happen .. ignore & hope space will appear
#ifdef LOG_ERR
      vc_con << VCFL;
      Form( vc_con, "write( %d, %p, %d ) = %d", Desc, Buff, Count, cnt ) << endl << VCER;
#endif

      return Count; // we can get out of memory too unless we remove it!
    }

#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "write( %d, %p, %d ) = %d", Desc, Buff, Count, cnt ) << endl << VCER;
#endif
  }

  return cnt; // propagate the result
}

//

struct statfs* System::fs_stat( ccptr Path )
{
  TSTAT;
  
  static struct statfs statfs_buf;

  errno = 0;

  sint4 ok = ::statfs( Path, &statfs_buf );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "statfs( " << Path << ", ... ) = " << ok << endl << VCER;
#endif
    return 0;
  }
  
  return &statfs_buf;
}

struct sysinfo* System::sysinfo()
{
  TSTAT;
  
  static struct sysinfo sysinfo_buf;

  errno = 0;

  sint4 ok = ::sysinfo( &sysinfo_buf );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "sysinfo( ... ) = " << ok << endl << VCER;
#endif
    return 0;
  }
  
  return &sysinfo_buf;
}

struct utsname* System::uname()
{
  TSTAT;
  
  static struct utsname utsname_buf;

  errno = 0;

  sint4 ok = ::uname( &utsname_buf );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "uname( ... ) = " << ok << endl << VCER;
#endif
    return 0;
  }
  
  return &utsname_buf;
}

//

sint4 System::sock_open( sint4 Domain, sint4 Type, sint4 Protocol )
{
  TSTAT;
  
  errno = 0;

  sint4 desc = socket( Domain, Type, Protocol );

  if ( desc < 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "socket( " << Domain << ", " << Type << ", " << Protocol << " ) = " << desc << endl << VCER;
#endif

    exit( errno );
  }

  return desc;
}

sint4 System::sock_connect( ccptr Host, uint4 Port, bool Exit )
{
  TSTAT;
  
  sint4 sock = sock_open( AF_INET, SOCK_STREAM, 0 );

  struct sockaddr_in sa;
  sint4                ip;

  bzero( &sa, sizeof(sa) );

  errno = 0;

  ip = inet_addr( Host );

  if ( ip == -1 ) {
    struct hostent* hp = gethostbyname( Host );
    if ( hp == 0 ) {
#ifdef LOG_ERR
      vc_con << VCFL
	     << "inet_addr( " << Host << " ) = " << ip << endl
	     << "gethostbyname( " << Host << " ) = " << hp << endl << VCER;
#endif
      if ( Exit ) exit( errno ); else return -1;
    }

    //  sa.sin_family = hp->h_addrtype;
    bcopy( hp->h_addr, cptr(&sa.sin_addr), hp->h_length );
  } else {
    bcopy( &ip, cptr(&sa.sin_addr), sizeof(ip) ); 
  }
  sa.sin_family = AF_INET;
  sa.sin_port   = htons((u_short) Port);

  sint4 ok = connect( sock, (struct sockaddr*) &sa, sizeof(sa) );

  if ( ok < 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "connect( " << sock << " ) = " << ok << endl << VCER;
#endif
    
    sock_close( sock );

    return -1;
  }

  return sock;
}

sint4 System::sock_accept( sint4 Sock )
{
  TSTAT;
  
  errno = 0;

  struct sockaddr_in sa_in;
  SOCKLEN_T         len = sizeof(sa_in);
  struct sockaddr*   sa = (struct sockaddr*) &sa_in;
  sint4 desc = ::accept( Sock, sa, &len );

  if ( desc < 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "accept( " << Sock << ", .. ) = " << desc << endl << VCER;
#endif
  }

  return desc;
}

void System::sock_bind( sint4 Sock, sint4 Domain, uint4 Port )
{
  TSTAT;
  
  int val;
  setsockopt( Sock, SOL_SOCKET, SO_REUSEADDR, &(val = 1), sizeof(val) );

  errno = 0;
  
  struct sockaddr_in sa_in; bzero( (cptr) &sa_in, sizeof(sa_in ) );
  sa_in.sin_family      = Domain;
  sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
  sa_in.sin_port        = htons( Port );

  struct sockaddr* sa = (struct sockaddr*) &sa_in;
  sint4             len = sizeof(             sa_in);

  sint4 ok = ::bind( Sock, sa, len );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "bind( %d, %p, %d ) = %d", Sock, sa, len, ok ) << endl << VCER;
#endif

    exit( errno );
  }
}

void System::sock_listen( sint4 Sock, sint4 OnHold )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = listen( Sock, OnHold );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "listen( " << Sock << ", " << OnHold << " ) = " << ok << endl << VCER;
#endif
    
    exit( errno );
  }
}

void System::sock_shutdown( sint4 Sock, sint4 How )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::shutdown( Sock, How  );

  if ( ok != 0 ) {
#if defined(LOG_ERR) && LOG > 2
    vc_con << VCFL << "shutdown( " << Sock << ", " << How << " ) = " << ok << endl << VCER;
#endif
  }
}

void System::sock_close( sint4 Sock )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::close( Sock );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "close( " << Sock << " ) = " << ok << endl << VCER;
#endif
  }
}

ccptr System::sock_stat( sint4 Sock )
{
  TSTAT;
  
  sint4 flag = System::fcntl( Sock, F_GETFL );

  if ( (flag & O_NONBLOCK) == O_NONBLOCK ) return "NON BLOCKING";

  return "BLOCKING";
}

//

void System::rlimit_get( RLIMIT_RESOURCE What, struct rlimit& rlim )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::getrlimit( What, &rlim );

  if ( ok == 0 ) return;

#ifdef LOG_ERR
  vc_con << VCFL << "getrlimit( " << int(What) << " ) = " << ok << endl << VCER;
#endif

  exit( errno );
}

void System::rlimit_set( RLIMIT_RESOURCE What, struct rlimit& rlim )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::setrlimit( What, &rlim );

  if ( ok == 0 ) return;

#ifdef LOG_ERR
  vc_con << VCFL << "setrlimit( " << int(What) << " ) = " << ok << endl << VCER;
#endif

  exit( errno );
}

void System::rusage_get( struct rusage& ru )
{
  errno = 0;

  sint4 ok = getrusage( RUSAGE_SELF, &ru );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "getrusage( RUSAGE_SELF, %p ) = %d", &ru, ok ) << endl << VCER;
#endif

    exit( errno );
  }
}

//

sint4 System::select_desc( sint4 N, fd_set& Read, fd_set& Write, fd_set& Excp, struct timeval* T )
{
  TSTAT;
  
  errno = 0;

  sint4 no = ::select( N, &Read, &Write, &Excp, T );

  if ( no < 0 ) {
    if ( errno == EAGAIN ) return no;
    if ( errno == EINTR  ) return no;
    if ( errno == 0      ) return no;

#ifdef LOG_ERR
    vc_con << VCFL << "select( " << N << ", ... ) = " << no << endl << VCER;
#endif

    exit( errno );
  }

  return no;
}

//

sint4 System::fcntl( sint4 Desc, sint4 Cmd )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::fcntl( Desc, Cmd );

  if ( ok == -1 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "fcntl( " << Desc << ", " << Cmd << " ) = " << ok << endl << VCER;
#endif

    exit( errno );
  }

  return ok;
}

sint4 System::fcntl( sint4 Desc, sint4 Cmd, long Arg )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::fcntl( Desc, Cmd, Arg );

  if ( ok == -1 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "fcntl( " << Desc << ", " << Cmd << ", " << Arg << " ) = " << ok << endl << VCER;
#endif

    exit( errno );
  }

  return ok;
}

//

sint4 System::getsockopt( sint4 Sock, sint4 Level, sint4 Opt, vptr Val, SOCKLEN_T* Len )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::getsockopt( Sock, Level, Opt, Val, Len );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "getsockopt( " << Sock << ", " << Level << ", " << Opt << ", .. , " << *Len << " )" << endl << VCER;
#endif

    exit( errno );
  }

  return ok;
}

sint4 System::setsockopt( sint4 Sock, sint4 Level, sint4 Opt, vptr Val, SOCKLEN_T Len )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::setsockopt( Sock, Level, Opt, Val, Len );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "setsockopt( " << Sock << ", " << Level << ", " << Opt << ", .. , " << Len << " )" << endl << VCER;
#endif

    exit( errno );
  }

  return ok;
}

void System::txtsockopt( sint4 Sock, ostream& os )
{
  TSTAT;
  
  sint4 val; SOCKLEN_T len;
  System::getsockopt( Sock, SOL_SOCKET, SO_DEBUG,     &val, &len );
  os << "SO_DEBUG    (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_REUSEADDR, &val, &len );
  os << "SO_REUSEADDR(" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_TYPE,      &val, & len );
  os << "SO_TYPE     (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_ERROR,     &val, & len );
  os << "SO_ERROR    (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_DONTROUTE, &val, &len );
  os << "SO_DONTROUTE(" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_BROADCAST, &val, & len );
  os << "SO_BROADCAST(" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_SNDBUF,    &val, & len );
  os << "SO_SNDBUF   (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_RCVBUF,    &val, &len );
  os << "SO_RCVBUF   (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_KEEPALIVE, &val, &len );
  os << "SO_KEEPALIVE(" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_OOBINLINE, &val, & len );
  os << "SO_OOBINLINE(" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_NO_CHECK,  &val, & len );
  os << "SO_NO_CHECK (" << Sock << ") = " << val << endl;

  System::getsockopt( Sock, SOL_SOCKET, SO_PRIORITY,  &val, & len );
  os << "SO_PRIORITY (" << Sock << ") = " << val << endl;

  struct linger ling = { 0, 0 };
  System::getsockopt( Sock, SOL_SOCKET, SO_LINGER,    &ling, &len );
  os << "SO_LINGER   (" << Sock << ") = {" << ling.l_onoff << ", " << ling.l_linger << "}" << endl;


  System::getsockopt( Sock, SOL_SOCKET, SO_BSDCOMPAT,  &val, & len );
  os << "SO_BSDCOMPAT(" << Sock << ") = " << val << endl;
}

//

sint4 System::getpeername( sint4 Sock, struct sockaddr* sa, SOCKLEN_T* Len )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::getpeername( Sock, sa, Len );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "getpeername( %d, %p, .. )", Sock, sa ) << endl << VCER;
#endif
  }

  return ok;
}

sint4 System::getsockname( sint4 Sock, struct sockaddr* sa, SOCKLEN_T* Len )
{
  TSTAT;
  
  errno = 0;

  sint4 ok = ::getsockname( Sock, sa, Len );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "getsockname( %d, %p, .. )", Sock, sa ) << endl << VCER;
#endif

    exit( errno );
  }

  return ok;
}

//

struct hostent* System::gethostbyaddr( const struct sockaddr* Addr, sint4 Len, sint4 Type )
{
  TSTAT;

  return 0; // !!! NO DNS (otherwise can get blocked!)

  errno = 0;

  struct hostent* he;
  he = ::gethostbyaddr( (ccptr) Addr, Len, Type );

  if ( he == 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL;
    Form( vc_con, "gethostbyaddr( %p, %d, %d ) = 0", Addr, Len, Type )
      << endl
      << "h_errno = " << h_errno << ' ' << hstrerror( h_errno ) << endl;
#endif
  }

  return he;
}

void System::gethostname( String& host_name )
{
  TSTAT;
  
  errno = 0;

  char name[ 1024 ];
  sint4 ok = ::gethostname( name, 1023 );

  if ( ok != 0 ) {
#ifdef LOG_ERR
    vc_con << VCFL << "gethostname( .. ) = " << ok << endl << VCER;
#endif

    exit( errno );
  }

  host_name = name;
}

void System::getsockhost( String& host_name, String& host_ip, sint4& Port, sint4 Sock )
{
  TSTAT;
  
  struct sockaddr_in sa_in; 
  struct sockaddr*   sa = (struct sockaddr*) &sa_in;
  SOCKLEN_T         len = sizeof             (sa_in);

#if   defined( PEER_NAME )
  sint4 ok = getpeername( Sock, sa, &len );
#elif defined( SOCK_NAME )
  sint4 ok = getsockname( Sock, sa, &len );
#else
#err "undefined"
error
#endif

  if ( ok != 0 ) { // we don't have anything !?!
    host_name = "*system failure*";
    host_ip   = "???.???.???.???";
    return;
  }

  uint4 nip = htonl( sa_in.sin_addr.s_addr);
  uint4 ip1 = (nip >> 24) & 0xff;
  uint4 ip2 = (nip >> 16) & 0xff;
  uint4 ip3 = (nip >>  8) & 0xff;
  uint4 ip4 = (nip      ) & 0xff;

  char h_ip[ 1024 ]; sprintf( h_ip, "%03d.%03d.%03d.%03d", ip1, ip2, ip3, ip4 ); host_ip = h_ip;
  Port = htons( sa_in.sin_port );

  struct hostent* he;
  sa  = (struct sockaddr*) &sa_in.sin_addr.s_addr;
  len = sizeof(             sa_in.sin_addr.s_addr);
  he = gethostbyaddr( sa, len, AF_INET );

  if ( he ) host_name = he->h_name; else host_name = "";
}

//: System.C (eof) (c) Igor
