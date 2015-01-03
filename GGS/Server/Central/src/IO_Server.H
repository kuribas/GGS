// $Id: IO_Server.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_Server.H (bof) (c) Igor Durdanovic

#ifndef IO_Server_H
#define IO_Server_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "IO.H"
#include "VEC.H"

class IO_Server
{
public:
   IO_Server();
  ~IO_Server();

  sint4 size() const { return len; } // number of used fds
  void  down(); // cleans everything
  void  full(); // cleans ids with full buffers
  void  loop();
  void  step( sint8 usec ); // < 0 = no return until event, no check for vc_sig.exit()
    
  sint4 last() const { return vec.size()-1; }

  void free_mux_clients( IO_TCP_Mux* Mux );
  void free_client( IO* Client, bool Error );

  void add_client ( IO& Client );
  void del_client ( IO& Client );

  void add_send   ( IO& Client );
  void add_recv   ( IO& Client );
  void add_excp   ( IO& Client );

  void del_send   ( IO& Client );
  void del_recv   ( IO& Client );
  void del_excp   ( IO& Client );

  const IO* operator [] ( uint4 i ) const { return vec[i]; }
  /* */ IO* operator [] ( uint4 i ) /* */ { return vec[i]; }
  
private:
  /* ctor */   IO_Server( const IO_Server& Server );
  IO_Server& operator = ( const IO_Server& Server );

  void sync(); // sync mux & log here, not everywhere in the source
  void oper( fd_set& fd_tmp, IO::OPER oper );

  VEC<IO*> vec;
  sint4    len;

  fd_set fd_send;
  fd_set fd_recv;
  fd_set fd_excp;
};

#endif

//: IO_Server.H (eof) (c) Igor
