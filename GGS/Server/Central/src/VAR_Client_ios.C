// $Id: VAR_Client_ios.C 9037 2010-07-06 04:05:44Z mburo $
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
//: VAR_Client_ios.C (bof) (c) Igor Durdanovic

#include "VAR_Client.H"
#include <cstdio>
#include <cstring>

void blank( char *m )
{
  int l = strlen(m);
  for ( int i = l; --i >= 0; ) if ( isspace(m[i]) ) m[i] = 0; else break;
  int n = 0;
  for ( ; m[n] != 0; n++ ) if (! isspace(m[n]) ) break;
  for ( int i = 0, j = n; ; i++, j++ ) if ( ( m[i] = m[j] ) == 0 ) break;
}

void VAR_Client::ios()
{
  FILE* f = fopen( "ios.user", "r" ); if ( f == 0 ) abort();
  char m[512];

  erase();
  
  for ( ;; ) {
    if ( feof( f ) ) break;
    fgets(m,512,f); blank(m); String Login = m;
    fgets(m,512,f); blank(m); String Passw = m;
    fgets(m,512,f); blank(m); String Name  = m;
    fgets(m,512,f); blank(m); String Email = m;
    make( Login, Passw, Name, Email );
    fgets(m,512,f); blank(m); info  = m; info.replace( '\\', "\ninfo  : " );
    fgets(m,512,f);
    fgets(m,512,f);
    fgets(m,512,f);
    for ( int i = 0; i < 10; i++ ) fgets(m,512,f);
    fgets(m,512,f);

    modified();
    save();
  }
}

//: VAR_Client_ios.C (eof) (c) Igor
