// $Id: Histogram_HDMY.H 160 2007-06-22 15:21:10Z mburo $
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
//: Histogram_HDMY.H (bof) (c) Igor Durdanovic

#ifndef Histogram_HDMY_H
#define Histogram_HDMY_H

#include "String.H"
#include "Histogram.H"

class Histogram_HDMY
{
public:
  Histogram_HDMY();

  enum TYPE { HOURLY, DAILY, MONTHLY, YEARLY };
  
  std::ostream& print( std::ostream& os, TYPE T ) const;

  void update( int Val );
  
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
private:
  static ccptr tag;
  
  Histogram _H; // 60 m(in)
  Histogram _D; // 24 H
  Histogram _M; // 31 D
  Histogram _Y; // 12 M
};

#endif

//: Histogram_HDMY.H (eof) (c) Igor
