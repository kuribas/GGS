// $Id: Signal.H 160 2007-06-22 15:21:10Z mburo $
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
//: Signal.H (bof) (c) Igor Durdanovic

#ifndef Signal_H
#define Signal_H

#include <signal.h>

class Signal
{
public:
   Signal();
  ~Signal();

  static bool    exit() { bool tmp = sig_exit; sig_exit = false; return tmp; }
  static bool    pipe() { bool tmp = sig_pipe; sig_pipe = false; return tmp; }
  static bool    alrm() { bool tmp = sig_alrm; sig_alrm = false; return tmp; }

  static void   block() { sigprocmask( SIG_BLOCK,   &sig_set, 0 ); }
  static void unblock() { sigprocmask( SIG_UNBLOCK, &sig_set, 0 ); }

  static void    send( sint4 Sig );

private:
  static void handler( sint4 SigNo );
  static void handler_EXIT();
  static void handler_PANIC();
  static void handler_SIGPIPE();
  static void handler_SIGALRM();
  static void handler_SIGCHLD();

  static sigset_t sig_set;
  static bool     sig_exit;
  static bool     sig_pipe;
  static bool     sig_alrm;
  static bool     sig_usr1;
};

#endif

//: Signal.H (eof) (c) Igor
