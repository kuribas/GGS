// $Id: CRatio.C 160 2007-06-22 15:21:10Z mburo $
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
//: CRatio.C (bof) (c) Igor Durdanovic

#include "COMMA.H"
#include "CRatio.H"
#include <iomanip>

#if defined(TSTAT_OK)
#include "TSTAT_Client.H"
#endif

using namespace std;

ostream& CRatio::print( ostream& os, ccptr fmt, sint4 tw, sint4 cw ) const
{
  TSTAT;
  
  real8 f = 0; if ( txt > 0 ) f = com * 100.0 / txt;
  Form( os, fmt, f )
    << setw(cw) << COMMA(com) << " / "
    << setw(tw) << COMMA(txt);
  return os;
}

//: CRatio.C (eof) (c) Igor
