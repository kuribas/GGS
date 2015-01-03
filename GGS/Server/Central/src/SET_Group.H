// $Id: SET_Group.H 160 2007-06-22 15:21:10Z mburo $
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
//: SET_Group.H (bof) (c) Igor Durdanovic

#ifndef SET_Group_H
#define SET_Group_H

#include "Group.H"
#include "SET.H"

class SET_Group
  : public SET<Group>
{
public:
  void vec( std::vector<String>& S ) const;
  
  bool add( const String&   Group,  const String& Login, bool Create );
  bool del( const String&   Group,  const String& Login, bool Remove );

  void add( const std::vector<String>& Groups, const String& Login, bool Create );
  void del( const std::vector<String>& Groups, const String& Login, bool Remove );

  std::ostream& print( std::ostream& os, bool Hide, bool NewLine ) const;
};

inline std::ostream& operator << ( std::ostream& os, const SET_Group& v )
{
  return v.print( os, true, false );
}

#endif

//: SET_Group.H (eof) (c) Igor
