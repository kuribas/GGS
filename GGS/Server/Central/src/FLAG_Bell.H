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

  bool parse( const String& S ); // (+|-)(t|tc|tg|n|ng|nc|ni|nn)

  bool tell()          const { return 0 != ( val & TELL ); }
  bool tell_chann()    const { return 0 != ( val & TELL_CHANN ); }
  bool tell_group()    const { return 0 != ( val & TELL_GROUP ); }
  bool notify()        const { return 0 != ( val & NOTIFY ); }
  bool notify_chann()  const { return 0 != ( val & NOTIFY_CHANN ); }
  bool notify_group()  const { return 0 != ( val & NOTIFY_GROUP ); }
  bool notify_ignore() const { return 0 != ( val & NOTIFY_IGNORE ); }
  bool notify_notify() const { return 0 != ( val & NOTIFY_NOTIFY ); }
    
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

private:
  uint4 val;

  enum { len = 8 };
  
  static ccptr name[ len ];
  
  enum { TELL          = 0x0001,
	 TELL_CHANN    = 0x0002,
	 TELL_GROUP    = 0x0004,
	 NOTIFY        = 0x0008,
	 NOTIFY_CHANN  = 0x0020,
	 NOTIFY_GROUP  = 0x0010,
	 NOTIFY_IGNORE = 0x0040,
	 NOTIFY_NOTIFY = 0x0080
  };
};

inline std::ostream& operator << ( std::ostream& os, const FLAG_Bell& v ) { return v.print( os ); }

#endif

//: FLAG_Bell.H (eof) (c) Igor
