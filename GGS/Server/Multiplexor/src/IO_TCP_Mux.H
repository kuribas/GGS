// $Id: IO_TCP_Mux.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Mux.H (bof) (c) Igor Durdanovic

#ifndef IO_TCP_Mux_H
#define IO_TCP_Mux_H

#include "String.H"
#include "IO_TCP.H"

class IO_TCP_Mux
  : public IO_TCP
{
public:
  IO_TCP_Mux( sint4 Desc, IO_Buffer::Method m ); 
  virtual ~IO_TCP_Mux();

  virtual void close( bool Err );
  virtual bool work ( IO_Buffer& B );

  enum TE_EVENT { TE_KILL = 0, TE_NO_PINGP, TE_PING };
  
protected:
  enum COMMANDS { CMD_PING = 0 };

  void send_ping();
  void recv_ping();
  
private:
  /* ctor */   IO_TCP_Mux( const IO_TCP_Mux& Client );
  IO_TCP_Mux& operator = ( const IO_TCP_Mux& Client );
};

#endif

//: IO_TCP_Mux.H (eof) (c) Igor
