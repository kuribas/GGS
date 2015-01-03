// $Id: bz2.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#include <stdio.h>
#include <iostream>
extern "C" {
#include <bzlib.h>
}

using namespace std;

const int MAX = 1 << 20;

char inp[ MAX ];
char out[ MAX ];
unsigned int  ilen;
unsigned int  olen;

int main() {
  int tinp = 0;
  int tout = 0;
  for ( ;; ) {
    cin.getline( inp, MAX );
    if ( cin.eof() ) { olen = 0; goto WRITE; }
    ilen = strlen( inp );
    inp[ ilen++ ]= '\n';
    inp[ ilen ] = 0;
    olen = MAX;
    BZ2_bzBuffToBuffCompress( out, &olen, inp, ilen, 1, 0, 30 );
  WRITE: ;
    tinp += ilen;
    tout += olen;
    unsigned short int ilen2 = ilen;
    unsigned short int olen2 = olen;
    cout.write( &olen2, sizeof(olen2) );
    cout.write( &ilen2, sizeof(ilen2) );
    if ( olen == 0 ) break;
    cout.write( out, olen );
  }
  cerr << tinp << " -> " << tout << endl;
  return 0;
}
