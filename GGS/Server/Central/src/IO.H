// $Id: IO.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO.H (bof) (c) Igor Durdanovic

#ifndef IO_H
#define IO_H

#include "IO_Buffer.H"
#include <sstream>

class IO_TCP_Mux;

class IO
  : public std::ostream
{
public:
  enum MSSG { CONNECT, DISCONNECT, MESSAGE, COMMAND };
  
  IO( sint4 Desc, IO_TCP_Mux* mux, bool Full,
      uint4 send_size, uint4 recv_size, uint4 excp_size, 
      bool Sync, bool Alloc, 
      IO_Buffer::Method m ); 

  virtual ~IO() {}

  typedef bool (IO::*OPER) ();

  virtual bool send () = 0; // sanity check of i/o mux
  virtual bool recv () = 0;
  virtual bool excp () = 0;
  virtual void close( bool Err ) = 0;
  virtual bool clog () = 0;

  sint4       io_desc()  const { return desc; }
  IO_TCP_Mux* io_mux()   const { return mux; }
  bool        io_full()  const { return full; }
  bool        io_sync()  const { return direct; }
  bool        io_alloc() const { return alloc; }
  bool        io_mark()  const { return marked; }
  IO&         io_mark( bool B ) { marked = B; return *this; }
  
  IO&  text( ccptr mssg, uint4 len );
  IO&  text( std::ostringstream& os );
  IO&  mssg( const String& file );

  int sync();

  static ccptr mssg_str( MSSG m ) { return MssgName[m]; }
  
protected:
  static char* MssgName[];

  IO_Buffer send_buff;
  IO_Buffer recv_buff;
  IO_Buffer excp_buff;

  sint4      desc;
  IO_TCP_Mux* mux;
  bool       full;
  bool     direct;
  bool      alloc;

  mutable bool marked;
  
private:
  /* ctor */   IO( const IO& Client );
  IO& operator = ( const IO& Client );

  friend class IO_Server;
  friend class IO_TCP_Mux;
  friend class VAR_Client;
};

#endif

//: IO.H (eof) (c) Igor
