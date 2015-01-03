// $Id: IO_TCP_Client.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Client.H (bof) (c) Igor Durdanovic

#ifndef IO_TCP_Client_H
#define IO_TCP_Client_H

#include "IO_TCP.H"

class IO_TCP_Mux;

class IO_TCP_Client
  : public IO_TCP
{
public:
  IO_TCP_Client( sint4 Desc, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m );
  virtual ~IO_TCP_Client();

  virtual void close( bool Err );
  virtual void time ( sint4 Mssg, uint4 Time, bool Set );
  virtual bool work ( IO_Buffer& B );

protected:
private:
  /* ctor */   IO_TCP_Client( const IO_TCP_Client& Client );
  IO_TCP_Client& operator = ( const IO_TCP_Client& Client );

  void adjust(); // adjust various sizes after user is on-line

  friend class PTR_Client;
};

#endif

//: IO_TCP_Client.H (eof) (c) Igor
