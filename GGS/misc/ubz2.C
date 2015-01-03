// $Id: ubz2.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#include <stdio.h>
#include <iostream>
extern "C" {
#include <bzlib.h>
}

const int MAX = 1 << 20;

char inp[ MAX ];
char out[ MAX ];
unsigned int  ilen;
unsigned int  olen;

int main() {
  for ( ;; ) {
    unsigned short int ilen2;
    unsigned short int olen2;
    cin.read( &ilen2, sizeof(ilen2) );
    cin.read( &olen2, sizeof(olen2) );
    ilen = ilen2;
    olen = olen2;
    cerr << ilen << " / " << olen << endl;
    if ( ilen == 0 ) break;
    cin.read( inp, ilen );
    olen = MAX;
    bzBuffToBuffDecompress( out, &olen, inp, ilen, 0, 0 );
    cout.write( out, olen );
  }
  return 0;
}
