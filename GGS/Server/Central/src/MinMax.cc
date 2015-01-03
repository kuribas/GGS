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
//: MinMax.cc (bof) (c) Igor Durdanovic

template < class T >
void MinMax<T>::update( int idx, const T& Val )
{
  if (! _ok || _min > Val ) _min = Val;
  if (! _ok || _max < Val ) _max = Val;
  _cur = Val;
  _idx = idx;
  _ok = true;
}

template < class T >
void MinMax<T>::reset()
{
  _ok  = false;
}
  
template < class T >
std::ostream& MinMax<T>::save ( std::ostream& os ) const
{
  os.write( ccptr(&_ok ), sizeof(_ok ) );
  os.write( ccptr(&_idx), sizeof(_idx) );
  os.write( ccptr(&_cur), sizeof(_cur) );
  os.write( ccptr(&_min), sizeof(_min) );
  os.write( ccptr(&_max), sizeof(_max) );

  return os;
}

template < class T >
std::istream& MinMax<T>::load ( std::istream& is )
{
  is.read( cptr(&_ok ), sizeof(_ok ) );
  is.read( cptr(&_idx), sizeof(_idx) );
  is.read( cptr(&_cur), sizeof(_cur) );
  is.read( cptr(&_min), sizeof(_min) );
  is.read( cptr(&_max), sizeof(_max) );

  return is;
}

//: MinMax.cc (eof) (c) Igor
