// $Id: VC_Match.H 160 2007-06-22 15:21:10Z mburo $
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
//: VC_Match.H (bof) (c) Igor Durdanovic

#ifndef VC_Match_H
#define VC_Match_H

#include "Game.H"
#include "String.H"
#include "VC_Request.H"
#include "TIME_Client.H"
#include "GAME_Clock.H"
#include "MatchType.H"


class Client;

class VC_Match
  : public TIME_Client
{
public:
  /*   */  VC_Match( const Request& R );
  virtual ~VC_Match();

  /* */ const String&      td() const { return req.td(); }  // tournament director
  /* */ const String&     tid() const { return req.tid(); } // tournament id
  /* */ const String&      id() const { return sid; } // should stick to strings
  /* */ const String& counter() const { return scnt; }
  /* */ bool            saved() const { return req.saved(); }
  /* */ bool            rated() const { return req.rated(); }
  const MatchType&       type() const { return req.type(); }
  const String&          name( uint4 i ) const;
  /* */ char       last_color( uint4 i ) const; // search history to find last color
  /* */ real8          rating( uint4 i ) const;
  /* */ real8          ratdev( uint4 i ) const;
  /* */ bool          trusted( uint4 i ) const;
  /* */ bool             dtor( uint4 i ) const;
  /* */ GAME_Clock&    clock0( uint4 i ) /* */ { return i == 0 ? req.clk(0) : req.clk(1); }
  const GAME_Clock&    clock0( uint4 i ) const { return i == 0 ? req.clk(0) : req.clk(1); }
  /* */ GAME_Clock&    clock1( uint4 i ) /* */ { return i == 0 ? req.clk(2) : req.clk(3); }
  const GAME_Clock&    clock1( uint4 i ) const { return i == 0 ? req.clk(2) : req.clk(3); }
  
  virtual bool play   ( std::ostream& os, const String& id, const String& P, const String& M ) = 0;
  virtual bool score  ( std::ostream& os, const String& id, const String& P, const String& M ) = 0; // ?
  virtual bool undo   ( std::ostream& os, const String& id, const String& P ) = 0;
  virtual bool resign ( std::ostream& os, const String& id, const String& P ) = 0;
  virtual void abort  ( const String& P ) = 0;
  virtual void adjourn( const String& P ) = 0;

  void cb_start() const; // game start call back
  void cb_update_to_all() const ;
  void cb_update_to_name(const String &name) const;
  void cb_update_to_all_but_name(const String &name) const;  
  void cb_end() const; // game end

  typedef void (VC_Match::*MssgFun)( std::ostream& os, bool Client ) const;
  typedef void (VC_Match::*CallFun)( const String& Name ) const;
  
  virtual void mssg_last(std::ostream& os, bool Client) const = 0; // for every player
  virtual void call_join(const String &name) const = 0; // notified of the event

  virtual void     te_handle( sint4 Mssg, uint4 Time ) = 0; // time-event handling
  virtual std::ostream& te_print ( std::ostream& os ) const;

  void cb_adjourn( const String& Record, const String& Info );
  void cb_archive( const String& Record, const String& Info,
		   real8 Result,        // result is for the first player
		   real8 NormResult );  // normalized result [0..1]

  std::ostream& print( std::ostream& os ) const;

protected:
  String match_str;

private:
  sint4      mid; // match id
  String     sid; // strings are more uniform then ints
  sint4     mcnt; // absoulute match counter
  String    scnt;
  Request    req;
  std::stringstream data;

  
  static SET_id<64> id_cnt;

  void refcnt( const String& key ); // updates refcnt for history games
  
  void clean_requests () const; // check open < play
  void clean_observers() const; // remove match id from all observers
  
  friend class EXE_Service;
  friend class VC_SEQ;
};

inline bool operator <  ( const VC_Match& A, const VC_Match& B )
{
  return A.id() <  B.id();
}
inline bool operator == ( const VC_Match& A, const VC_Match& B )
{
  return A.id() == B.id();
}
inline bool operator <  ( const VC_Match& A, const String&   B )
{
  return A.id() <  B;
}
inline bool operator == ( const VC_Match& A, const String&   B )
{
  return A.id() == B;
}

#endif

//: VC_Match.H (eof) (c) Igor
