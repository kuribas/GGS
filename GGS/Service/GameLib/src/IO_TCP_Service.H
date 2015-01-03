// $Id: IO_TCP_Service.H 160 2007-06-22 15:21:10Z mburo $
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
//: IO_TCP_Service.H (bof) (c) Igor Durdanovic

#ifndef IO_Client_TCP_Service_H
#define IO_Client_TCP_Service_H

#include "IO_TCP.H"
#include "TIME_Client.H"

class IO_TCP_Service
  : public IO_TCP,
    protected TIME_Client
{
public:
  IO_TCP_Service( sint4 desc, IO_Buffer::Method m );
  virtual ~IO_TCP_Service();

  virtual void close( bool Err );

  virtual void          te_handle( sint4 Mssg, uint4 Time );
  virtual std::ostream& te_print ( std::ostream& os ) const;

  virtual bool work ( IO_Buffer& B );

  static sint4 connect( ccptr Host, unsigned Port );

private:
  /* ctor */   IO_TCP_Service( const IO_TCP_Service& Client );
  IO_TCP_Service& operator = ( const IO_TCP_Service& Client );

  enum { skip, ready } state;
};

#endif

//: IO_TCP_Service.H (eof) (c) Igor
