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
#include "TIME_Client.H"
#include "VEC.H"

class IO_TCP_Client;

class IO_TCP_Mux
  : public IO_TCP,
    public TIME_Client
{
public:
  IO_TCP_Mux( sint4 Desc, IO_Buffer::Method m, const String& Key ); 
  virtual ~IO_TCP_Mux();

  virtual void close( bool Err );
  virtual bool work ( IO_Buffer& B );

  void remove( sint4 Desc );

  virtual void          te_handle( sint4 Mssg, uint4 Time );
  virtual std::ostream& te_print ( std::ostream& os ) const;
  
  const String& operator() () const { return key; }
  
  static IO_TCP_Mux* connect( ccptr Host, sint4 Port, IO_Buffer::Method m, const String Key );

  std::ostream& print( std::ostream& os ) const;

  enum TE_EVENT { TE_KILL = 0, TE_NO_PING, TE_PING };
  
protected:
  enum COMMANDS { CMD_PING = 0 };

  void te_ping();
  void te_no_ping();
  
  void send_ping();
  void recv_ping();
  
  String key;
  String host_ip;
  String host_name;
  sint4  host_port;
  
private:
  /* ctor */   IO_TCP_Mux( const IO_TCP_Mux& Client );
  IO_TCP_Mux& operator = ( const IO_TCP_Mux& Client );

  VEC<IO_TCP_Client*> vec; // list of muxed clients
};

// this is needed for SET<>
inline bool operator <  ( const IO_TCP_Mux& p, const IO_TCP_Mux& q ) { return p() <  q(); }
inline bool operator == ( const IO_TCP_Mux& p, const IO_TCP_Mux& q ) { return p() == q(); }

inline bool operator <  ( const IO_TCP_Mux& p, const String& q ) { return p() <  q; }
inline bool operator == ( const IO_TCP_Mux& p, const String& q ) { return p() == q; }

#endif

//: IO_TCP_Mux.H (eof) (c) Igor
