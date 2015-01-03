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
//: IO.cc (bof) (c) Igor Durdanovic

template <class T>
IO& operator << ( IO& io, const T& V )
{
  if ( &io == &vc_con && !vc_con_ready ) return io;
  if ( &io == &vc_log && !vc_log_ready ) return io;
  reinterpret_cast<ostream&>( io ) << V; 
  io.emit();
  return io;
}

template <class T>
IO& operator << ( IO& io, T V )
{
  if ( &io == &vc_con && !vc_con_ready ) return io;
  if ( &io == &vc_log && !vc_log_ready ) return io;
  reinterpret_cast<ostream&>( io ) << V; 
  io.emit();
  return io;
}

//: IO.cc (eof) (c) Igor
