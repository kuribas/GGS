// $Id: TIME_Server.H 160 2007-06-22 15:21:10Z mburo $
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
//: TIME_Server.H (bof) (c) Igor Durdanovic

#ifndef TIME_Server_H
#define TIME_Server_H

#include "TIME_Client.H"

#include <vector>

// use new mothod for scheduling time & i/o events suggested at Bloomberg
#define NEW_METHOD

class String;

class TIME_Event
{
public:
  TIME_Event( TIME_Client* Client = 0, sint4 Mssg = 0, uint4 Time = 0 )
    : client(Client), mssg(Mssg), time(Time) {}

  bool operator < ( const TIME_Event& E ) const { return time > E.time; } // smaller at the end

  std::ostream& print( std::ostream& os ) const;
  
private:
  TIME_Client* client;
  sint4        mssg;
  uint4        time;

  friend class TIME_Server;
};

inline std::ostream& operator << ( std::ostream& os, const TIME_Event&  v ) { return v.print( os ); }

class TIME_Server
{
public:
   TIME_Server();
  ~TIME_Server();

#ifdef NEW_METHOD	
	sint8 next() const; // how many us before next time event, -1 for no events
#endif	
  void  time();

  std::ostream& print( std::ostream& os ) const;
  
private:
#ifndef NEW_METHOD
	void alarm(); // sets alarm
#endif
	
  unsigned add( TIME_Client* Client, sint4 Mssg, uint4 Time, bool Abs = false );
  void     del( TIME_Client* Client, sint4 Mssg );
  void     del( TIME_Client* Client );

  typedef std::vector< TIME_Event > Queue;
  typedef std::vector< TIME_Event >::const_iterator const_iterator;
  
  Queue queue;

  uint4          size()  const { return queue.size(); }
  const_iterator begin() const { return queue.begin(); }
  const_iterator end()   const { return queue.end(); }

  friend std::ostream& operator << ( std::ostream& os, const TIME_Server& V );
  friend class TIME_Client;
};

inline std::ostream& operator << ( std::ostream& os, const TIME_Server& v ) { return v.print( os ); }

#endif

//: TIME_Server.H (eof) (c) Igor
