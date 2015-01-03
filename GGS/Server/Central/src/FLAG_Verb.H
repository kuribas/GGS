// $Id: FLAG_Verb.H 160 2007-06-22 15:21:10Z mburo $
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
//: FLAG_Verb.H (bof) (c) Igor Durdanovic

#ifndef FLAG_Verb_H
#define FLAG_Verb_H

#include "String.H"

class FLAG_Verb
{ 
public:
  FLAG_Verb() : val(0xFFFF) {}

  bool parse( const String& S ); // (+|-)(news|ack|help|faq)

  bool news() const { return 0 != ( val & NEWS ); }
  bool ack () const { return 0 != ( val & ACK  ); }
  bool help() const { return 0 != ( val & HELP ); }
  bool faq () const { return 0 != ( val & FAQ  ); }
    
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );

private:
  uint4 val;

  enum { len = 4 };
  
  static ccptr name[ len ];
  
  enum { NEWS = 0x0001,
	 ACK  = 0x0002,
	 HELP = 0x0004,
	 FAQ  = 0x0008
  };
};

inline std::ostream& operator << ( std::ostream& os, const FLAG_Verb& v ) { return v.print( os ); }

#endif

//: FLAG_Verb.H (eof) (c) Igor
