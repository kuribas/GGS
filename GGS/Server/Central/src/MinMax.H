// $Id: MinMax.H 160 2007-06-22 15:21:10Z mburo $
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
//: MinMax.H (bof) (c) Igor Durdanovic

#ifndef MinMax_H
#define MinMax_H

template < class T >
class MinMax
{
public:
  MinMax() : _ok(false), _min(0), _max(0) {}
  
  T   min() const { return _min; }
  T   max() const { return _max; }
  T   cur() const { return _cur; }
  int idx() const { return _idx; }
  bool ok() const { return _ok; }
    
  void update( int idx, const T& Val );
  void reset();
  
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
private:
  bool _ok;
  int  _idx;
  T    _cur;
  T    _min;
  T    _max;
};

#include "MinMax.cc"

#endif

//: MinMax.H (eof) (c) Igor
