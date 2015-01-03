// $Id: Histogram.H 160 2007-06-22 15:21:10Z mburo $
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
//: Histogram.H (bof) (c) Igor Durdanovic

#ifndef Histogram_H
#define Histogram_H

#include "CPP.H"
#include "MinMax.H"
#include "VEC.H"
#include "String.H"

class Histogram
{
public:
  Histogram( int N );

  void update( int i, sint8 Val );

  std::ostream& print( std::ostream& os, ccptr* Name = 0 ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
private:
  static const String tag;
  
  VEC< MinMax<sint8> >  arr;
  MinMax<sint8>         minmax;
  size_t                curr_i;
};

#endif

//: Histogram.H (eof) (c) Igor
