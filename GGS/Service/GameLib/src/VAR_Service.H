// $Id: VAR_Service.H 160 2007-06-22 15:21:10Z mburo $
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
//: VAR_Service.H (bof) (c) Igor Durdanovic

#ifndef VAR_Service_H
#define VAR_Service_H

#include "TIME_Client.H"
#include "SET_Alias.H"
#include "SET_Group.H"
#include "SET_String.H"
#include "VAR_Client.H"
#include "IO_TCP_Service.H"
#include "SET_Rank.H"
#include "GAME_History.H"
#include "SET_PTR_Client.H"
#include "SET_PTR_Match.H"
#include "SET_PTR_Request.H"
#include "Histogram_HDMY.H"

class VAR_Service
  : public TIME_Client
{
public:
  /*   */  VAR_Service();
  virtual ~VAR_Service();

  void connect ( ccptr Host, sint4 Port, IO_Buffer::Method m );

  void modified() { dirty = true; }
  void erase();
  void init();
  void sync(); // io sync of all clients!
  void save( std::ostream& os ) const;
  void save() const;
  void load();

  static bool  name_ok( const String& Name );

  void add( Client* Who );
  void del( Client* Who );

  virtual void          te_handle( sint4 Mssg, uint4 Time ); // schedule save
  virtual std::ostream& te_print ( std::ostream& os ) const;

  static bool ready( const String& S );
  
  Client*  client( const String& Login, bool Mak_Ghost, bool Ret_Ghost );
  VAR_Client* var( const String& Login ); // ptr to static svar!

  bool connected() { return socket != 0 && socket->io_desc() >= 0; };

  void set_match_cnt(sint4 cnt) { match_cnt = cnt; }
  
protected:
  IO_TCP_Service* socket;

  sint4           match_cnt; // permanent
  SET_Alias       alias;
  SET_Rank        rank;
  GAME_History    history;
  SET_Group       groups;
  Histogram_HDMY  histo_users;
  Histogram_HDMY  histo_requests;
  Histogram_HDMY  histo_matches;
  Histogram_HDMY  histo_inp_bytes;
  Histogram_HDMY  histo_out_bytes;
  
  SET_PTR_Client clients;  // all clients
  SET_PTR_Match  matches;  // all matches in progress
  SET_PTR_Request global;  // all global requests
  SET_PTR_Request  local;  // all local  requests

  SET_Group       ignore;
  SET_Group        watch;
  SET_Group        track;
  SET_Group       notify;
  SET_String star_notify;
  SET_String  star_track;
  mutable bool     dirty;

private:
  static VAR_Client svar;

  friend bool IO_TCP_Service::work( IO_Buffer& B );

  friend class Client;
  friend class VC_Match;
  friend class VC_Request;
  friend class EXE_Service;
  friend class VC_SEQ;
  friend class VAR_Client; // need to rate IOS games
  friend class Stat;
  friend void db_format( DB_Server& db, const String& key );
  friend class Rebuild_Rank; // TODO: to many friends .. find better solution
  friend void  rebuild_rank();  
};

#endif

//: VAR_Service.H (eof) (c) Igor
