// $Id: EXE_Client.H 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Client.H (bof) (c) Igor Durdanovic

#ifndef EXE_Client_H
#define EXE_Client_H

#include "SET_Group.H"

class Histogram_HDMY;
class IO_TCP_Client;

class EXE_Client
{
public:
   EXE_Client();
  ~EXE_Client();

  void operator() ( IO_TCP_Client& who, String& text );

  static void recv_notify        ( /* */ IO_TCP_Client& who ); // upon log on!
  static void recv_star_notify   ( /* */ IO_TCP_Client& who );
  static void recv_service_notify( /* */ IO_TCP_Client& who );
  
  static void send_notify        ( const IO_TCP_Client& who, bool Arrived ); // incremental
  static void send_star_notify   ( const IO_TCP_Client& who, bool Arrived ); 
  static void send_service_notify( const IO_TCP_Client& who, bool Arrived ); 

private:
  typedef void (EXE_Client::*Command) ( IO_TCP_Client& who, const String& text );

  class EXE_Cmd  : public NAMED<Command>
  {
  public:
    EXE_Cmd( const String& Name, Command Cmd );
    
    std::ostream& print( std::ostream& os ) const;
  };
  
  class EXE_Cmds : public SET< EXE_Cmd >
  {
  public:
    std::ostream& print( std::ostream& os ) const;
  };

  void shell_execute( IO_TCP_Client& who, String& line, bool Prefix );
  void shell_replace( IO_TCP_Client& who, String& line, sint4  no_rec );
  void shell_expand ( IO_TCP_Client& who, String& line,
		      sint4& no_cmd, sint4 no_rec, bool Prefix );
  
  void alias    ( IO_TCP_Client& who, const String& text );
  void var      ( IO_TCP_Client& who, const String& text );
  void bell     ( IO_TCP_Client& who, const String& text );
  void vt100    ( IO_TCP_Client& who, const String& text );
  void hear     ( IO_TCP_Client& who, const String& text );
  void verbose  ( IO_TCP_Client& who, const String& text );
  void info     ( IO_TCP_Client& who, const String& text );
  void passw    ( IO_TCP_Client& who, const String& text );
      
  void history  ( IO_TCP_Client& who, const String& text );
  void who      ( IO_TCP_Client& who, const String& text );
  void finger   ( IO_TCP_Client& who, const String& text );
      
  void echo     ( IO_TCP_Client& who, const String& text );
  void tell     ( IO_TCP_Client& who, const String& text );
  void quit     ( IO_TCP_Client& who, const String& text );
      
  void chann    ( IO_TCP_Client& who, const String& text );
  void group    ( IO_TCP_Client& who, const String& text );
  void notify   ( IO_TCP_Client& who, const String& text );
  void ignore   ( IO_TCP_Client& who, const String& text );

  void repeat   ( IO_TCP_Client& who, const String& text );
public:
  void help     ( IO_TCP_Client& who, const String& text );
private:
  void name     ( IO_TCP_Client& who, const String& text );
  void email    ( IO_TCP_Client& who, const String& text );
      
  void histo_inp( IO_TCP_Client& who, const String& text );
  void histo_out( IO_TCP_Client& who, const String& text );
  void histo_usr( IO_TCP_Client& who, const String& text );

  void uptime   ( IO_TCP_Client& who, const String& text );
  void shistory ( IO_TCP_Client& who, const String& text );
  void salias   ( IO_TCP_Client& who, const String& text );
  void down     ( IO_TCP_Client& who, const String& text );
  void mux      ( IO_TCP_Client& who, const String& text );
  void pack     ( IO_TCP_Client& who, const String& text );
  void exec     ( IO_TCP_Client& who, const String& text );
  void tstat    ( IO_TCP_Client& who, const String& text );
  void trace    ( IO_TCP_Client& who, const String& text );
  void tevent   ( IO_TCP_Client& who, const String& text );

  void dummy    ( IO_TCP_Client& who, const String& text );
  
  EXE_Cmds cmds;

  void add( cptr Name, Command Proc );

  void tell_send( IO_TCP_Client& who, const String& text );

  void histo( IO_TCP_Client& who, const String& text, const Histogram_HDMY& H, ccptr Title );

  static bool add_group( IO_TCP_Client& who, const String& user, const String& group );
  static bool del_group( IO_TCP_Client& who, const String& user, const String& group );
  
  static void notify_group ( const     String& G, const String& U, const IO_TCP_Client* P, ccptr Fix );
  static void notify_chann ( const     String& C, /*            */ const IO_TCP_Client* P, ccptr Fix );
  static void notify_chann ( const SET_String& S, /*            */ const IO_TCP_Client* P, ccptr Fix );
  static void notify_ignore( const     String& C, /*            */ const IO_TCP_Client* P, ccptr Fix );
  static void notify_ignore( const SET_String& S, /*            */ const IO_TCP_Client* P, ccptr Fix );
  static void   recv_notify( const SET_String& S, /*            */       IO_TCP_Client& who );
  static void notify_notify( const     String& C, /*            */ const IO_TCP_Client* P, ccptr Fix );
  static void notify_notify( const SET_String& S, /*            */ const IO_TCP_Client* P, ccptr Fix );

  friend inline std::ostream& operator << ( std::ostream& os, const EXE_Client::EXE_Cmd&  x ) { return x.print( os ); }
  friend inline std::ostream& operator << ( std::ostream& os, const EXE_Client::EXE_Cmds& x ) { return x.print( os ); }
};

#endif

//: EXE_Client.H (eof) (c) Igor
