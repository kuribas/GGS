// $Id: IO_TCP.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP.H (bof) (c) Igor Durdanovic

#ifndef IO_TCP_H
#define IO_TCP_H

#include "IO.H"

class IO_TCP_Mux;

class IO_TCP
  : public IO
{
public:
  IO_TCP( bool Server, sint4 Desc, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m );
  virtual ~IO_TCP();

  virtual bool send(); // will be called by vc_ios
  virtual bool recv();
  virtual bool excp();
  virtual void close( bool Err ); // in case of err and static obj
  virtual bool clog() { return false; }

  virtual bool work( IO_Buffer& B ) = 0; // will be called by recv() after lo_recv()

protected:
  bool  server;
  uint4 send_size;
  uint4 recv_size;

  virtual bool lo_send( IO_Buffer& B, uint4 Flag ); // will be called by send()
  virtual bool lo_recv( IO_Buffer& B, uint4 Flag ); // will be called by recv()

private:
  /* ctor */   IO_TCP( const IO_TCP& Client );
  IO_TCP& operator = ( const IO_TCP& Client );
};

#endif

//: IO_TCP.H (eof) (c) Igor
