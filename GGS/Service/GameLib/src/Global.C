// $Id: Global.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

#include "Global.H"
#include <time.h>

using namespace std;

bool Rand::use_time_seed = true;

// generate random integer >= 0 && < limit

sint4 Rand::num(sint4 limit)
{
  static bool first_call = true;

  assert(limit > 0);

  if (use_time_seed && first_call) {
    first_call = false;
    srand48(time(0)); // used by STL!
    srandom(time(0));
    srand(time(0));
  }

  return (sint4) (limit*double(random())/(double(RAND_MAX)+1.0));
}


sint4 Color::opponent(sint4 col) {
  assert(col == BLACK || col == WHITE);
  if (col == BLACK) return WHITE; else return BLACK;
}


void Color::write(ostream &os, sint4 col) {
  if      (col == UNDEF) os << '?';
  else if (col == BLACK) os << '*';
  else if (col == WHITE) os << 'O';
  else ERR("illegal color");
}


sint4 Color::read(istream &is) {
  char c;
  is >> c;
  // errstr << char(c) << endl;
  if      (c == '*') return BLACK;
  else if (c == 'O') return WHITE;
  else if (c == '?') return UNDEF;
  else ERR("illegal color");
  return -1;
}

