// $Id: EXE_Service.H 160 2007-06-22 15:21:10Z mburo $
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
//: EXE_Service.H (bof) (c) Igor Durdanovic

#ifndef EXE_Service_H
#define EXE_Service_H

#include "NAMED.H"
#include "SET.H"
#include "VEC.H"

class Client;
class VC_Match;
class VC_Request;
class Histogram_HDMY;

class EXE_Service
{
public:
   EXE_Service();
  ~EXE_Service();

  void operator() ( Client* P, String& arg );

  void match_start  ( const VC_Match& M ); // match async events
  void match_update_to_all          ( const VC_Match& M );
  void match_update_to_name         ( const VC_Match& M, const String& N );
  void match_update_to_all_but_name ( const VC_Match& M, const String& N );
  void match_end    ( const VC_Match& M );
  void match_join   ( const VC_Match& M );
  
private:
  typedef void (EXE_Service::*Command) ( Client* P, const String& arg );

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

  void shell_execute( Client* P, String& line );
  void shell_expand ( Client* P, String& line, sint4& no_cmd, sint4 no_rec );


  void add        ( Client* P, const String& arg );
  void del        ( Client* P, const String& arg );

  void top        ( Client* P, const String& arg );
  void rank       ( Client* P, const String& arg );
       
  void who        ( Client* P, const String& arg );
  void finger     ( Client* P, const String& arg );
  void assess     ( Client* P, const String& arg );

  void group      ( Client* P, const String& arg );
  void tdtype     ( Client* P, const String& arg );
  void tdstart    ( Client* P, const String& arg );
  void tdadjourn  ( Client* P, const String& arg );
  void tdabort    ( Client* P, const String& arg );

  void ask        ( Client* P, const String& arg );
  void cont       ( Client* P, const String& arg );
  void request    ( Client* P, const String& arg );
  void accept     ( Client* P, const String& arg );
  void decline    ( Client* P, const String& arg );
  void cancel     ( Client* P, const String& arg );

  void aform      ( Client* P, const String& arg );
  void dform      ( Client* P, const String& arg );
  
  void play       ( Client* P, const String& arg );
  void undo       ( Client* P, const String& arg );
  void abort      ( Client* P, const String& arg );
  void adjourn    ( Client* P, const String& arg );
  void score      ( Client* P, const String& arg );
  void resign     ( Client* P, const String& arg );
  void moves      ( Client* P, const String& arg );

  void tell       ( Client* P, const String& arg );

  void look       ( Client* P, const String& arg );
  void history    ( Client* P, const String& arg );
  void stored     ( Client* P, const String& arg );
       
  void help       ( Client* P, const String& arg );
       
  void open       ( Client* P, const String& arg );
  void client     ( Client* P, const String& arg );

  void bell       ( Client* P, const String& arg );
  void vt100      ( Client* P, const String& arg );
  void hear       ( Client* P, const String& arg );
  void trust      ( Client* P, const String& arg );
  void rated      ( Client* P, const String& arg );
       
  void watch      ( Client* P, const String& arg );
  void match      ( Client* P, const String& arg );
  void ignore     ( Client* P, const String& arg );
  void notify     ( Client* P, const String& arg );
  void track      ( Client* P, const String& arg );
       
  void histo_inp  ( Client* who, const String& text );
  void histo_out  ( Client* who, const String& text );
  void histo_req  ( Client* who, const String& text );
  void histo_mat  ( Client* who, const String& text );
  void histo_usr  ( Client* who, const String& text );

  void uptime     ( Client* P, const String& arg );
  void salias     ( Client* P, const String& arg );
  void shistory   ( Client* P, const String& arg );
  void pack       ( Client* P, const String& arg );
  void del_stored ( Client* P, const String& arg );
  void tstat      ( Client* P, const String& arg );
  void tevent     ( Client* P, const String& arg );

  EXE_Cmds cmds;

  void add( ccptr Name, Command Proc );

  bool add_group( const String& user, const String& group );
  bool del_group( const String& user, const String& group );
  
  typedef VEC< const VC_Request* >               VecReqPtr;

  void histo( Client* who, const String& text, const Histogram_HDMY& H, ccptr Title );

  void request_glob ( VecReqPtr& A, Client* M, Client* O );
  void request_recv ( VecReqPtr& A, Client* M, Client* O, const SET_String& S );
  void request_send ( VecReqPtr& A, Client* M, Client* O, const SET_String& S );
  void request_print( VecReqPtr& A, Client* M );
  void watch_mssg   ( VC_Match&  M, std::ostringstream& mssg );

  void notify_ignore( const     String& C, const Client* P, ccptr Fix );
  void notify_ignore( const SET_String& S, const Client* P, ccptr Fix );
  void notify_notify( const     String& C, const Client* P, ccptr Fix );
  void notify_notify( const SET_String& S, const Client* P, ccptr Fix );
  void notify_track ( const     String& C, const Client* P, ccptr Fix );
  void notify_track ( const SET_String& S, const Client* P, ccptr Fix );
  void notify_watch ( const     String& C, const Client* P, ccptr Fix, Client* WP );
  void notify_others_watch ( const     String& C, const Client* P, ccptr Fix );
  void notify_player_watch ( const     String& C,       Client* P, ccptr Fix );
  void notify_others_watch ( const SET_String& S, const Client* P, ccptr Fix );
  void notify_player_watch ( const SET_String& S,       Client* P, ccptr Fix );
  void notify_stored( Client* P );

  void star_assess( Client* P, /*                 */ const String& Login2, const String& Score );
  void star_assess( Client* P, const String& Login1, const String& Login2, const String& Score );
  
  friend class Client;
  friend inline std::ostream& operator << ( std::ostream& os, const EXE_Cmd&  x )
  {
    return x.print( os );
  }
  friend inline std::ostream& operator << ( std::ostream& os, const EXE_Cmds& x )
  {
    return x.print( os );
  }
};

#endif

//: EXE_Service.H (eof) (c) Igor
