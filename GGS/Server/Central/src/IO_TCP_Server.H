// $Id: IO_TCP_Server.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Server.H (bof) (c) Igor Durdanovic

#ifndef IO_TCP_Server_H
#define IO_TCP_Server_H

#include "IO_TCP.H"

class IO_TCP_Server
  : protected IO_TCP
{
public:
  IO_TCP_Server( sint4 Port, IO_Buffer::Method m );
  virtual ~IO_TCP_Server();

  virtual bool recv();

private:
  IO_Buffer::Method method;

  virtual bool send() { return true; }
  virtual bool excp() { return true; }

  virtual bool work( IO_Buffer& /*B*/ ) { return true; }

  /* ctor */   IO_TCP_Server( const IO_TCP_Server& Server );
  IO_TCP_Server& operator = ( const IO_TCP_Server& Server );
};

#endif

//: IO_TCP_Server.H (eof) (c) Igor
