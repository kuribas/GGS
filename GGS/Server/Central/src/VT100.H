// $Id: VT100.H 160 2007-06-22 15:21:10Z mburo $
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
//: VT100.H (bof) (c) Igor Durdanovic

#ifndef VT100_H
#define VT100_H

#include "CPP.H"
#include <iostream>

#define vt_bell      ""
#define vt_interrupt "[2K\015"

// global modifiers
#define vt_reset      "[0m"
#define vt_bold       "[5m"
#define vt_underline  "[4m"
#define vt_reverse    "[7m"
		     
// foreground low-lite
#define vt_FLred      "[31m" 
#define vt_FLgreen    "[32m"
#define vt_FLyellow   "[33m"
#define vt_FLblue     "[34m"
#define vt_FLpink     "[35m"
#define vt_FLcyan     "[36m"
#define vt_FLwhite    "[37m"

// background lo-lite
#define vt_BLred      "[41m"
#define vt_BLgreen    "[42m"
#define vt_BLyellow   "[43m"
#define vt_BLblue     "[44m"
#define vt_BLpink     "[45m"
#define vt_BLcyan     "[46m"
#define vt_BLwhite    "[47m"

// foreground hi-lite
#define vt_FHred      "[91m"
#define vt_FHgreen    "[92m"
#define vt_FHyellow   "[93m"
#define vt_FHblue     "[94m"
#define vt_FHpink     "[95m"
#define vt_FHcyan     "[96m"
#define vt_FHwhite    "[97m"

// foreground hi-lite
#define vt_BHred      "[101m"
#define vt_BHgreen    "[102m"
#define vt_BHyellow   "[103m"
#define vt_BHblue     "[104m"
#define vt_BHpink     "[105m"
#define vt_BHcyan     "[106m"
#define vt_BHwhite    "[107m"


// x = IO_TCP_Client

// global modifiers
#define VT_reset(x)      if ( (x).var.vt100 ) x << vt_reset
#define VT_bold(x)       if ( (x).var.vt100 ) x << vt_bold
#define VT_underline(x)  if ( (x).var.vt100 ) x << vt_underline
#define VT_reverse(x)    if ( (x).var.vt100 ) x << vt_reverse
		     
// foreground low-lite
#define VT_FLred(x)      if ( (x).var.vt100 ) x << vt_FLred
#define VT_FLgreen(x)    if ( (x).var.vt100 ) x << vt_FLgreen
#define VT_FLyellow(x)   if ( (x).var.vt100 ) x << vt_FLyellow
#define VT_FLblue(x)     if ( (x).var.vt100 ) x << vt_FLblue
#define VT_FLpink(x)     if ( (x).var.vt100 ) x << vt_FLpink
#define VT_FLcyan(x)     if ( (x).var.vt100 ) x << vt_FLcyan
#define VT_FLwhite(x)    if ( (x).var.vt100 ) x << vt_FLwhite

// background lo-lite
#define VT_BLred(x)      if ( (x).var.vt100 ) x << vt_BLred
#define VT_BLgreen(x)    if ( (x).var.vt100 ) x << vt_BLgreen
#define VT_BLyellow(x)   if ( (x).var.vt100 ) x << vt_BLyellow
#define VT_BLblue(x)     if ( (x).var.vt100 ) x << vt_BLblue
#define VT_BLpink(x)     if ( (x).var.vt100 ) x << vt_BLpink
#define VT_BLcyan(x)     if ( (x).var.vt100 ) x << vt_BLcyan
#define VT_BLwhite(x)    if ( (x).var.vt100 ) x << vt_BLwhite

// foreground hi-lite
#define VT_FHred(x)      if ( (x).var.vt100 ) x << vt_FHred
#define VT_FHgreen(x)    if ( (x).var.vt100 ) x << vt_FHgreen
#define VT_FHyellow(x)   if ( (x).var.vt100 ) x << vt_FHyellow
#define VT_FHblue(x)     if ( (x).var.vt100 ) x << vt_FHblue
#define VT_FHpink(x)     if ( (x).var.vt100 ) x << vt_FHpink
#define VT_FHcyan(x)     if ( (x).var.vt100 ) x << vt_FHcyan
#define VT_FHwhite(x)    if ( (x).var.vt100 ) x << vt_FHwhite

// foreground hi-lite
#define VT_BHred(x)      if ( (x).var.vt100 ) x << vt_BHred
#define VT_BHgreen(x)    if ( (x).var.vt100 ) x << vt_BHgreen
#define VT_BHyellow(x)   if ( (x).var.vt100 ) x << vt_BHyellow
#define VT_BHblue(x)     if ( (x).var.vt100 ) x << vt_BHblue
#define VT_BHpink(x)     if ( (x).var.vt100 ) x << vt_BHpink
#define VT_BHcyan(x)     if ( (x).var.vt100 ) x << vt_BHcyan
#define VT_BHwhite(x)    if ( (x).var.vt100 ) x << vt_BHwhite


// macros
#define vt_ready vt_BLgreen  vt_reverse
#define vt_alert vt_BHyellow vt_reverse
#define vt_self  vt_bold
#define vt_error vt_BHred    vt_reverse
#define vt_start vt_BLcyan   vt_reverse

#endif

//: VT100.H (eof) (c) Igor
