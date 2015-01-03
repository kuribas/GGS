// $Id: FLAG_Bell.H 160 2007-06-22 15:21:10Z mburo $
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
//: FLAG_Bell.H (bof) (c) Igor Durdanovic

#ifndef FLAG_Bell_H
#define FLAG_Bell_H

#include "String.H"

class FLAG_Bell
{ 
public:
  FLAG_Bell() : val(0) {}

  bool parse( const String& S ); // (+|-)(r|p|w|n|ns|nn|nt|ni|nr|nw|ta|to|tp)

  bool request()        const { return 0 != ( val & REQUEST ); }
  bool play()           const { return 0 != ( val & PLAY ); }
  bool watch()          const { return 0 != ( val & WATCH ); }
  bool notify_match()   const { return 0 != ( val & NOTIFY_MATCH ); }
  bool notify_stored()  const { return 0 != ( val & NOTIFY_STORED ); }
  bool notify_notify()  const { return 0 != ( val & NOTIFY_NOTIFY ); }
  bool notify_track()   const { return 0 != ( val & NOTIFY_TRACK ); }
  bool notify_ignore()  const { return 0 != ( val & NOTIFY_IGNORE ); }
  bool notify_request() const { return 0 != ( val & NOTIFY_REQUEST ); }
  bool notify_watch()   const { return 0 != ( val & NOTIFY_WATCH ); }
  bool tell_all()       const { return 0 != ( val & TELL_ALL ); }
  bool tell_observers() const { return 0 != ( val & TELL_OBSERVERS ); }
  bool tell_players()   const { return 0 != ( val & TELL_PLAYERS ); }
    
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

private:
  uint4 val;

  enum { len = 13 };
  
  static ccptr name[ len ];
  
  enum { REQUEST        = 0x0001,
	 PLAY           = 0x0002,
	 WATCH          = 0x0004,
	 NOTIFY_MATCH   = 0x0008,
	 NOTIFY_STORED  = 0x0010,
	 NOTIFY_NOTIFY  = 0x0020,
	 NOTIFY_TRACK   = 0x0040,
	 NOTIFY_IGNORE  = 0x0080,
	 NOTIFY_REQUEST = 0x0100,
	 NOTIFY_WATCH   = 0x0200,
	 TELL_ALL       = 0x0400,
	 TELL_OBSERVERS = 0x0800,
	 TELL_PLAYERS   = 0x1000
  };
};

inline std::ostream& operator << ( std::ostream& os, const FLAG_Bell& v )
{
  return v.print( os );
}

#endif

//: FLAG_Bell.H (eof) (c) Igor
