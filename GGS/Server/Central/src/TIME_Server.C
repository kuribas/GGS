// $Id: TIME_Server.C 160 2007-06-22 15:21:10Z mburo $
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
//: TIME_Server.C (bof) (c) Igor Durdanovic

#include "System.H"
#include "Signal.H"
#include "Actors.H"
#include "String.H"
#include "IO_FILE.H"
#include "TIME_Server.H"
#include <algorithm>
#include <sstream>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& TIME_Event::print( ostream& os ) const
{
  TSTAT;
  
  client->te_print( os << System::dtime( time ) << ' ' << mssg << ' ' );

  return os;
}

TIME_Server::TIME_Server()
{
  vc_con << VCTIME << "  TIME_Server()" << endl;
}

TIME_Server::~TIME_Server()
{
#ifndef NEW_METHOD
	::alarm( 0 );
#endif
  queue.erase( queue.begin(), queue.end() );

  vc_con << VCTIME << " ~TIME_Server()" << endl;
}

uint4 TIME_Server::add( TIME_Client* Client, sint4 Mssg, uint4 Time, bool Abs )
{
  TSTAT;
  
  if ( Client == 0 ) { vc_con << VCFL; return 0; }
  
  if (! Abs ) Time += System::clock();

  TIME_Event e( Client, Mssg, Time );

  Queue::iterator up = upper_bound( queue.begin(), queue.end(), e );
  Queue::iterator lo = lower_bound( queue.begin(), queue.end(), e );

  for ( Queue::iterator it = lo ; it != up; ++it ) { 
    if ( (*it).client == Client && (*it).mssg == Mssg && (*it).time == Time )
      return Time; // check for duplicates
  }

#ifndef NEW_METHOD
	bool schedule = up == queue.end();
#endif
	
  queue.insert( up, e );

#ifndef NEW_METHOD
	if ( schedule ) alarm();
#endif
  
  return Time;
}

void TIME_Server::del( TIME_Client* Client, sint4 Mssg )
{
  TSTAT;
  
  if ( queue.size() == 0 ) return;

#ifndef NEW_METHOD
	bool schedule = ( queue.back().client == Client && queue.back().mssg == Mssg );
#endif
	
  for ( sint4 i = queue.size(); --i >= 0; ) {
    if ( queue[i].client == Client && queue[i].mssg == Mssg )
      queue.erase( queue.begin() + i );
  }
#ifndef NEW_METHOD
	if ( schedule ) alarm();
#endif	
}

void TIME_Server::del( TIME_Client* Client )
{
  TSTAT;

  if ( queue.size() == 0 ) return;

#ifndef NEW_METHOD
	bool schedule = ( queue.back().client == Client );
#endif
	
  for ( sint4 i = queue.size(); --i >= 0; ) {
    if ( queue[i].client == Client )
      queue.erase( queue.begin() + i );
  }

#ifndef NEW_METHOD
  if ( schedule ) alarm();
#endif	
}

sint8 TIME_Server::next() const
{
	if ( queue.empty() ) return -1;
	uint4 now = System::clock();
	if ( queue.back().time < now ) return 0;
	return (sint8(queue.back().time) - sint8(now)) * 1000000LL;
}

void TIME_Server::time()
{
  TSTAT;
  
  uint4 now = System::clock();

  while ( queue.size() && queue.back().time <= now ) {
    TIME_Event e = queue.back();
    queue.pop_back();
    if ( e.client == 0 ) { vc_con << VCFL; continue; }
    e.client->te_handle( e.mssg, e.time );
  }

#ifndef NEW_METHOD
	alarm();
#endif	
}

#ifndef NEW_METHOD
void TIME_Server::alarm()
{
  TSTAT;
  
  if ( queue.size() ) {
    uint4 now = System::clock();
    
    if ( queue.back().time <= now ) vc_sig.send( SIGALRM ); // send signal
    else                            ::alarm( queue.back().time - now );
  } else {
    ::alarm( 0 );
  }
}
#endif

ostream& TIME_Server::print( ostream& os ) const
{
  TSTAT;
  
  const_iterator lo = begin();
  const_iterator hi = end();
  os << size();
  for ( ; lo != hi; ++lo ) os << EOL << (*lo);

  return os;
}

#if 0
ostream& TIME_Server::print_gdb( ostream& os ) const
{
  TSTAT;
  
  const_iterator lo = begin();
  const_iterator hi = end();
  os << size();
  for ( ; lo != hi; ++lo ) {
    os << endl;
    os.form("%08x %8d %8d : ",
	    (*lo).client,
	    (*lo).mssg,
	    (*lo).time ) << flush;
    (*lo).client->te_print( os << System::dtime( (*lo).time ) << ' ' );
  }
  return os;
}
#endif

//: TIME_Server.C (eof) (c) Igor
