// $Id: Magic.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#ifndef Magic_H
#define Magic_H

#include "Global.H"

class Magic {

  enum { ALIVE = 0xab1c298f, DEAD = 0x98ab1cef };
  uint4 magic_foo;

public:
  
  Magic()  { magic_foo = ALIVE; }

  ~Magic() {
    check_magic();
    magic_foo = DEAD;
  }

  void check_magic() {
    if (magic_foo == DEAD) ERR("obj already dead!");
    if (magic_foo != ALIVE) ERR("obj overwritten!");
  }
};

#endif
