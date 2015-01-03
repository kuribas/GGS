// $Id: SET_PTR_Mux.C 160 2007-06-22 15:21:10Z mburo $
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
//: SET_PTR_Mux.C (bof) (c) Igor Durdanovic

#include "SET_PTR_Mux.H"

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

void SET_PTR_Mux::sync()
{
  TSTAT;
  
  iterator it = begin();
  iterator hi = end();
  for ( ; it != hi; ++it ) (*it)->sync();
}

ostream& SET_PTR_Mux::print( ostream& os ) const
{
  TSTAT;
  
  const_iterator it = begin();
  const_iterator hi = end();
  for ( ; it != hi; ++it ) (*it)->print( os ) << EOL;

  return os;
}

//: SET_PTR_Mux.C (eof) (c) Igor
