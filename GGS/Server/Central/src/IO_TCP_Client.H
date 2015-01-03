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
#include "TIME_Client.H"
#include "VAR_Client.H"

class IO_TCP_Client
  : public IO_TCP,
    protected TIME_Client
{
public:
  IO_TCP_Client( sint4 Desc, IO_TCP_Mux* Mux, bool Full, IO_Buffer::Method m );
  virtual ~IO_TCP_Client();

  virtual void close( bool Err );

  virtual void          te_handle( sint4 Mssg, uint4 Time );
  virtual std::ostream& te_print ( std::ostream& os ) const;

  virtual bool work ( IO_Buffer& B );
  
  IO_TCP_Client& bell     ( bool On );
  IO_TCP_Client& ready    ();
  IO_TCP_Client& interrupt();

  bool           vt100() const { return var.vt100; }
  IO_TCP_Client& vt100( const String& prefix, ccptr               mssg );
  IO_TCP_Client& vt100( const String& prefix, const String&       mssg );
  IO_TCP_Client& vt100( const String& prefix, std::ostringstream& mssg );
  
  const String& id() const { return var.login; }

  bool ignore( const IO_TCP_Client* whom ) const;

protected:
  VAR_Client var;
  String host_name;
  String host_ip;
  sint4  port;
  
private:
  /* ctor */   IO_TCP_Client( const IO_TCP_Client& Client );
  IO_TCP_Client& operator = ( const IO_TCP_Client& Client );

  enum { wait_login, wait_passwd, on_line } state;

  void adjust(); // adjust various sizes after user is on-line

  friend class VAR_System;
  friend class PTR_Client;
  friend class EXE_Client;
  friend class IO_TCP_Mux;
  friend class VC_SEQ;
};

inline bool operator <  ( const IO_TCP_Client& p, const IO_TCP_Client& q ) { return p.id() <  q.id(); }
inline bool operator == ( const IO_TCP_Client& p, const IO_TCP_Client& q ) { return p.id() == q.id(); }

inline bool operator <  ( const IO_TCP_Client& p, const String& q ) { return p.id() <  q; }
inline bool operator == ( const IO_TCP_Client& p, const String& q ) { return p.id() == q; }

#endif

//: IO_TCP_Client.H (eof) (c) Igor
