// $Id: Array_N.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#ifndef Array_N_H
#define Array_N_H

#include <cassert>

template < class T, int N >
class Array_N
{
private:
  T arr[N];

public:
  const T& operator[] ( int i ) const { assert( i >= 0 && i < N ); return arr[i]; }
  /* */ T& operator[] ( int i ) /* */ { assert( i >= 0 && i < N ); return arr[i]; }
};

#endif
