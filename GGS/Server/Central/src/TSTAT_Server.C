// $Id: TSTAT_Server.C 160 2007-06-22 15:21:10Z mburo $
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
//: TSTAT_Server.C (bof) (c) Igor Durdanovic

#include "TSTAT_Server.H"

using namespace std;

void TSTAT_Server::operator += ( const TSTAT_Client& C )
{
  TSTAT_Client* sc = (*this)( C );
  if ( sc == 0 ) SET<TSTAT_Client>::operator+=( C );
  else sc->accumulate( C );
}

ostream& TSTAT_Server::print( ostream& os ) const
{
  const_iterator it = begin();
  const_iterator hi = end();
  os << size();
  for ( ; it != hi; ++it ) os << EOL << *it;
  
  return os;
}

//: TSTAT_Server.C (eof) (c) Igor
