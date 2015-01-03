// $Id: GoHandicap.H 160 2007-06-22 15:21:10Z mburo $
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
//: (C) Igor Durdanovic - [bof] - GoHandicap.H

#ifndef GoHandicap_H
#define GoHandicap_H

#include <CPP.H>

class GoHandicap
{
public:
  GoHandicap( real8 RatingBlack,
	      real8 RatingWhite,
	      real8 KomiVal );

  static real8 MinKomi() { return  1E-3; }
  static real8 MoveVal() { return 100.0; }
    
  real8 Komi () const { return komi - score; }

  real8 Bscore() const { return score; } // black score - komi adjustment
  sint4 Bmoves() const { return moves; } // extra free moves for black

  real8 Brating( real8 RatingBlack ) const;
  
private:
  real8 komi;
  sint4 moves;
  real8 score;
};

#endif

//: (C) Igor Durdanovic - [eof] - GoHandicap.H
