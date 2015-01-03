// $Id: ChessImpl.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Michael Buro, mic@research.nj.nec.com
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

#include "RegularBoardGame.H"
#include "ChessImpl.H"
#include <algorithm>

using namespace std;

sint4 BoardType::board_width(sint4 t)
{
 //  strerr << (sint4)t << endl;

  if (t == TYPE_6)  return 6;
  if (t == TYPE_8)  return 8;
  if (t == TYPE_10) return 10;
  if (t == TYPE_12) return 12;  

  return -1; // illegal
}

sint4 BoardType::board_squares(sint4 t)
{
  //  strerr << (sint4)t << endl;
  
  if (t == TYPE_6)  return 36;
  if (t == TYPE_8)  return 64;
  if (t == TYPE_10) return 100;
  if (t == TYPE_12) return 144;  

  return -1; // illegal
}


bool BoardType::legal_rand_type(sint4 /*t*/, sint4 rand_type)
{
  return rand_type < 0;
}


//-------------------------------------------------------------------


sint4 Move::xy_to_ind(sint4 x, sint4 y)
{
  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);    
  return (y+2) * BoardType::DX + x + 2;  
}


void Move::ind_to_xy(sint4 ind, sint4 &x, sint4 &y)
{
  assert(ind >= 0 && ind < BoardType::MAX_BOARD_SIZE);
  y = ind / BoardType::DX - 2;
  x = ind % BoardType::DX - 2;

  assert(x >= 0 && x < BoardType::MAX_BOARD_WIDTH);
  assert(y >= 0 && y < BoardType::MAX_BOARD_WIDTH);
}


Move::Move(sint4 sq1)
{
  from = sq1;
}


// parse ascii-move [e.g. A7-A8Q]
// return true iff syntax correct

bool Move::parse(ostream &os, const String &s)
{
  from = to = MV_UNDEF;
  prom = NO_PROM;
    
  vector<String> vec;
  String::parse(s, vec, '-');

  if (vec.size() != 2) {
    os << "illegal coords; e.g. a1-a3";
    return false;
  }

  sint4 coords[2];
  
  FORT (i, 2) {

    istringstream iss(vec[i]);

    if (!Move::read_square(iss, coords[i])) {
      os << "illegal square";
      return false;
    }
  }

  from = coords[0];
  to   = coords[1];

  // check v[1] for promotion

  uint4 i;
  for (i=1; i < vec[1].size(); ++i) {
    if (isdigit(vec[1][i-1]) && !isdigit(vec[1][i])) break;
  }

  if (i < vec[1].size()) {

    // Promotion?

    if (i != vec[1].size()-1) return false;

    switch (toupper(vec[1][i])) {
    case 'N': prom = PROM_TO_KNIGHT; break;
    case 'B': prom = PROM_TO_BISHOP; break;
    case 'R': prom = PROM_TO_ROOK;   break;
    case 'Q': prom = PROM_TO_QUEEN;  break;
    default: return false;
    }
  }
  
  return true;
}


bool Move::is_valid() const
{
  return
    from >= 0 && from < BoardType::MAX_BOARD_SIZE &&
    to >= 0 && to < BoardType::MAX_BOARD_SIZE &&
    from != to;
}


void Move::write_square(ostream &os, sint4 sq)
{
  if (sq < 0) os << "%";
  else {
    int x, y;
    ind_to_xy(sq, x, y);
    Form(os, "%c%d", 'A'+x, y+1);
  }
}

bool Move::read_square(istream &is, sint4 &sq)
{
  char c;

  is >> c;
  if (!is) return false;

  if (c == '?' || c == '%') { sq = -1; return true; }

  int x = toupper(c) - 'A';
  if (x < 0 || x >= BoardType::MAX_BOARD_WIDTH) return false;

  int y;
  is >> y;
  if (!is) return false;
  --y;
  if (y < 0 || y >= BoardType::MAX_BOARD_WIDTH) return false;

  sq = xy_to_ind(x, y);

  //errsttr << "SQUARE=" << sq << " x=" << x << " y=" << y << endl;
  
  return true;
}


void Move::write(ostream &os) const
{
  if (from == MV_UNDEF) {

    os << "?";

  } else {

    write_square(os, from);
    os << "-";
    write_square(os, to);

    switch (prom) {
    case PROM_TO_KNIGHT: os << 'N'; break; 
    case PROM_TO_BISHOP: os << 'B'; break;
    case PROM_TO_ROOK:   os << 'R'; break;
    case PROM_TO_QUEEN:  os << 'Q'; break;
    default: break;
    }
  }
}


//-------------------------------------------------------------------


const int Board::d[8] =
{
  +1, -1,
  BoardType::DX, -BoardType::DX,
  BoardType::DX+1, -(BoardType::DX+1),
  BoardType::DX-1, -(BoardType::DX-1)
};


bool Board::sufficient_material(sint4 col) const {

  if (count_pieces(make_cont(PAWN, col)) > 0) return true;
  if (count_pieces(make_cont(ROOK, col)) > 0) return true;
  if (count_pieces(make_cont(QUEEN, col)) > 0) return true;

  int b = count_pieces(make_cont(BISHOP, col));
  if (b >= 2) return true; // must be of different color though

  int n = count_pieces(make_cont(KNIGHT, col));
  if (n >= 3) return true;

  if (b >= 1 && n >= 1) return true;
  return false;
}



void Board::regular_setup()
{
  sint4 w = BoardType::board_width(type);
  sint4 rank6[6] = { ROOK, KNIGHT, QUEEN, KING, BISHOP, ROOK };
  //sint4 rank6[6] = { ROOK, EMPTY, EMPTY, KING, EMPTY, ROOK };
  sint4 rank8[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
  sint4 rank10[10] = { ROOK, BISHOP, KNIGHT, BISHOP, QUEEN,
			 KING, BISHOP, KNIGHT, KNIGHT, ROOK };
  sint4 rank12[12] = { ROOK, KNIGHT, BISHOP, KNIGHT, BISHOP, QUEEN,
			 KING, BISHOP, KNIGHT, BISHOP, KNIGHT, ROOK };  
  
  assert((w & 1) == 0);

  // allow castling

  FORS (i, 2) {
    CK[i] = true;
    CQ[i] = true;
  }

  sint4 j = 0;
  FORS (i, w) {
    squares[Move::xy_to_ind(j, 1)]   = W_PAWN;
    squares[Move::xy_to_ind(j, w-2)] = B_PAWN;
    ++j;
  }

  sint4 *rank;

  switch (type) {
  case BoardType::TYPE_6:  rank = rank6; break;
  case BoardType::TYPE_8:  rank = rank8; break;
  case BoardType::TYPE_10: rank = rank10; break;
  case BoardType::TYPE_12: rank = rank12; break;  
  default: ERR("illegal type");
  }
  
  j = 0;

  FORS (i, w) {
    squares[Move::xy_to_ind(j, 0)]   = SqCont(rank[i]);
    squares[Move::xy_to_ind(j, w-1)] = SqCont(-rank[i]);
    ++j;
  }

#if 0

  // handicap : remove one white knight

  FORS (i, w) {
    if (squares[Move::xy_to_ind(i,0)]   == W_KNIGHT) {
      squares[Move::xy_to_ind(i,0)] = EMPTY;
      break;
    }
  }

#endif

  turn_color = WHITE;
}


void Board::random_setup(int /*ra*/)
{
  regular_setup();

  sint4 w = BoardType::board_width(type);;

  // scramble base lines

  bool ok = false;

  do {

    vector<sint4> v1;
    FORS (i, w) v1.push_back(squares[Move::xy_to_ind(i,0)]);
    random_shuffle(v1.begin(), v1.end());
    FORS (i, w) squares[Move::xy_to_ind(i,0)] = SqCont(v1[i]);
    
    vector<sint4> v2;
    FORS (i, w) v2.push_back(squares[Move::xy_to_ind(i,w-1)]);
    random_shuffle(v2.begin(), v2.end());
    FORS (i, w) squares[Move::xy_to_ind(i,w-1)] = SqCont(v2[i]);

    // count bishop color distribution;
    
    sint4 bb_b = 0, wb_b = 0;

    FORS (i, w) {
      if (squares[Move::xy_to_ind(i,0)]   == W_BISHOP && (i & 1) == 1) ++wb_b;
      if (squares[Move::xy_to_ind(i,w-1)] == B_BISHOP && (i & 1) == 0) ++bb_b;
    }
      
    //    errsttr << "RAND: " << bb_b << " " << wb_b << endl;
    ok = bb_b == wb_b; 

  } while (!ok);
  
  // castling enabled if pieces are on their standard places

  CQ[WHITE] = squares[ll_sq()] == W_ROOK && squares[w/2] == W_KING;
  CK[WHITE] = squares[lr_sq()] == W_ROOK && squares[w/2] == W_KING;
  CQ[BLACK] = squares[ul_sq()] == B_ROOK && squares[w/2] == B_KING;
  CK[BLACK] = squares[ur_sq()] == B_ROOK && squares[w/2] == B_KING;

  turn_color = WHITE;
}


// ra < 0 => regular setup, otherwise random_setup

void Board::init(sint4 bt, sint4 ra)
{
  type = bt;
  
  sint4 w = BoardType::board_width(type);

  // no castling
  
  FORT (i, 2) {
    CK[i] = false;
    CQ[i] = false;
  }

  ep_square = -1;
  rev_move_num = 0;
  
  FORT (i, (sint4)BoardType::MAX_BOARD_SIZE) squares[i] = BORDER;

  FORT (y, w) {
    FORT (x, w) {
      squares[Move::xy_to_ind(x, y)] = EMPTY;
    }
  }

  if (ra < 0) regular_setup(); else random_setup(ra);
}


sint4 Board::get_to_move() const
{
  return turn_color;
}


sint4 Board::opponent(sint4 col) {
  if (col == BLACK) return WHITE;
  return BLACK;
}

void Board::toggle_to_move()
{
  turn_color = opponent(turn_color);
} 


bool Board::is_finished() const
{
  MoveList ml;

  legal_moves(turn_color, ml);
  return ml.size() == 0;
}


sint4 Board::count_pieces(sint4 piece) const
{
  int n = 0;
  sint4 w = BoardType::board_width(type);

  FORT (y, w) {
    FORT (x, w) {
      if (squares[Move::xy_to_ind(x, y)] == piece) ++n;
    }
  }

  return n;
}


sint4 Board::blacks_result() const
{
  assert(is_finished());
  sint4 r;
  if (in_check(turn_color)) r = -1; else r = 0;
  if (turn_color == WHITE) r = -r;
  return r;
}


void Board::write_cont(ostream &os, sint4 cont) {
  
  switch (cont) {
  case EMPTY:    os << '-'; break;
  case B_PAWN:   os << 'p'; break;
  case B_KNIGHT: os << 'n'; break;
  case B_BISHOP: os << 'b'; break;
  case B_ROOK:   os << 'r'; break;
  case B_QUEEN:  os << 'q'; break;
  case B_KING:   os << 'k'; break;
       
  case W_PAWN:   os << 'P'; break;
  case W_KNIGHT: os << 'N'; break;
  case W_BISHOP: os << 'B'; break;
  case W_ROOK:   os << 'R'; break;
  case W_QUEEN:  os << 'Q'; break;
  case W_KING:   os << 'K'; break;

  default: os << '?';
  }
}

  
bool Board::read_cont(istream &is, SqCont &cont) {

  char c;

  is >> c;
  if (!is) return false;
  
  cont = BORDER;
  
  switch (c) {
  case '-': cont = EMPTY; break;
  case 'p': cont = B_PAWN;    break;
  case 'n': cont = B_KNIGHT;  break;
  case 'b': cont = B_BISHOP;  break;
  case 'r': cont = B_ROOK;    break;
  case 'q': cont = B_QUEEN;   break;
  case 'k': cont = B_KING;    break;
          		     
  case 'P': cont = W_PAWN;    break;
  case 'N': cont = W_KNIGHT;  break;
  case 'B': cont = W_BISHOP;  break;
  case 'R': cont = W_ROOK;    break;
  case 'Q': cont = W_QUEEN;   break;
  case 'K': cont = W_KING;    break;

  default: return false;
  }

  return true;
}



  
void Board::write_ggf(ostream &os, bool one_line) const
{
  sint4 w = BoardType::board_width(type);

  os << type << " ";

  Move::write_square(os, ep_square);

  os << " " << CQ[WHITE] << " " << CK[WHITE] << " "
     << CQ[BLACK] << " " << CK[BLACK];
  
  if (!one_line) os << EOL; else os << " ";

  FORT (y, w) {
    FORT (x, w) {
      write_cont(os, squares[Move::xy_to_ind(x, y)]);
    }

    if (one_line) os << " "; else os << EOL;
  }

  if (get_to_move() == BLACK) os << "*"; else os << "O";
}


bool Board::read_ggf(istream &is)
{
  char c;
  String s;
  
  is >> type;
  if (!is) return false;

  init(type, 0);

  if (!Move::read_square(is, ep_square)) return false;

  is >> CQ[WHITE];
  is >> CK[WHITE];
  is >> CQ[BLACK];
  is >> CK[BLACK];
   
  sint4 w = BoardType::board_width(type);

  FORT (y, w) {
    FORT (x, w) {
      if (!read_cont(is, squares[Move::xy_to_ind(x,y)])) return false;
    }
  }

  is >> c;
  if (c == '*') turn_color = BLACK;
  else if (c == 'O') turn_color = WHITE;
  else ERR("illegal color to move");

  return true;
}


void Board::write(ostream &os) const
{
  sint4 w = BoardType::board_width(type);
  
  os <<  "RM#="  << rev_move_num; 
  os << " EPS="; Move::write_square(os, ep_square);
  os << " CQW=" << CQ[WHITE];
  os << " CKW=" << CK[WHITE];
  os << " CQB=" << CQ[BLACK];  
  os << " CKB=" << CK[BLACK];
  os << EOL << EOL;

  os << "  ";
  FORT (x, w) Form( os, " %c", 'A'+x);
  os << EOL;
      
  for (sint4 y=w-1; y >= 0; --y) {
    Form( os, "%2d", y+1);
    FORT (x, w) {
      sint4 cont = squares[Move::xy_to_ind(x,y)];

      os << " ";

      if (cont == EMPTY) {
	if ((x+y) & 1) os << "-"; else os << " ";
      } else {
	write_cont(os, cont);
      }
    }
    Form( os, " %-2d", y+1);
    os << EOL;
  }

  os << "  ";
  FORT (x, w) Form( os, " %c", 'A'+x);
  os << EOL << EOL;

  if (get_to_move() == BLACK) os << "* to move"; else os << "O to move";
  os << EOL;
}


const int Board::UP_ONE_RANK_FOR_COLOR[2] = { N, S };

const int Board::HORI_VERT_D[4] = { N,  S,  W,  E };

const int Board::DIAG_D[4]    = { NW, NE, SW, SE };

const int Board::KING_D[8]    = { N,  S,  W,  E, NE, NW, SE, SW }; 

const int Board::KNIGHT_D[8] = { 
  +2*N+1*W, 
  +1*N+2*W, 
  +2*N-1*W, 
  +1*N-2*W, 
  -2*N+1*W, 
  -1*N+2*W, 
  -2*N-1*W, 
  -1*N-2*W 
}; 


bool Board::in_check(sint4 col) const
{
  bool a[2];

  attacked(king_loc(col), a);
  return a[opponent(col)];
}


void Board::add_normal_move(MoveList &ml, sint4 from, sint4 to) 
{
  Move m;

  m.from = from;
  m.to   = to;
  m.prom = Move::NO_PROM;
  ml.push_back(m);
}


void Board::moves_on_circle(
  sint4 col,
  MoveList &ml,
  sint4     sq,
  const int d[8]
) const
{
  int i;

  FOR (i, 8) {
    sint4 sq2 = sq+d[i];

    sint4 contf = squares[sq2];

    if (contf == EMPTY || (is_man(contf) && !has_color(contf, col))) 
      add_normal_move(ml, sq, sq2);
  }
}

void Board::moves_on_line(
  sint4 col,
  MoveList &ml,
  sint4     sq,
  int      d
) const
{
  sint4 sq2 = sq;
  sint4 contf;

  FOREVER {
    
    sq2 += d;
    contf = squares[sq2];
    if (contf == BORDER || (is_man(contf) && has_color(contf, col))) break;
    add_normal_move(ml, sq, sq2);    
    if (contf != EMPTY) break;
  }
}

#define FOR_SQ(i) \
  for (sint4 i=0; i < sint4(BoardType::MAX_BOARD_SIZE); ++i) \
    if (squares[i] != BORDER)

void Board::pseudo_legal_moves(sint4 col, MoveList &ml) const
{
  sint4 pc, d, kl;

  ml.erase(ml.begin(), ml.end());

  FOR_SQ (sq) { 

    sint4 contf = squares[sq];

    if (is_man(contf) && has_color(contf, col)) {
      pc = abs(contf);

      switch (pc) {

      case PAWN  :
	d = UP_ONE_RANK_FOR_COLOR[col];

	if (is_man(squares[sq+d+W]) && 
	    has_color(squares[sq+d+W], opponent(col))) {

	  add_normal_move(ml, sq, sq+d+W);  // capture 
	      
	  if (squares[sq+d+d] == BORDER) {  // promotions 

	    ml.back().prom = Move::PROM_TO_QUEEN;
	    add_normal_move(ml, sq, sq+d+W);
	    ml.back().prom = Move::PROM_TO_ROOK;
	    add_normal_move(ml, sq, sq+d+W);
	    ml.back().prom = Move::PROM_TO_BISHOP;
	    add_normal_move(ml, sq, sq+d+W);
	    ml.back().prom = Move::PROM_TO_KNIGHT;

	  }
	}

	if (is_man(squares[sq+d+E]) && 
	    has_color(squares[sq+d+E], opponent(col))) {

	  add_normal_move(ml, sq, sq+d+E);  // capture

	  if (squares[sq+d+d] == BORDER) {  // promotions

	    ml.back().prom = Move::PROM_TO_QUEEN;
	    add_normal_move(ml, sq, sq+d+E);
	    ml.back().prom = Move::PROM_TO_ROOK;
	    add_normal_move(ml, sq, sq+d+E);
	    ml.back().prom = Move::PROM_TO_BISHOP;
	    add_normal_move(ml, sq, sq+d+E);
	    ml.back().prom = Move::PROM_TO_KNIGHT;

	  }
	}
            
	if (sq+d+W == ep_square) {
	  add_normal_move(ml, sq, sq+d+W);  // ep-capture
	}
	
	if (sq+d+E == ep_square) {
	  add_normal_move(ml, sq, sq+d+E);  // ep-capture //
	}

	if (squares[sq+d] == EMPTY) {
	  add_normal_move(ml, sq, sq+d);
	  
	  if (squares[sq+d+d] == BORDER) { // promotions

	    ml.back().prom = Move::PROM_TO_QUEEN;
	    add_normal_move(ml, sq, sq+d);
	    ml.back().prom = Move::PROM_TO_ROOK;
	    add_normal_move(ml, sq, sq+d);
	    ml.back().prom = Move::PROM_TO_BISHOP;
	    add_normal_move(ml, sq, sq+d);
	    ml.back().prom = Move::PROM_TO_KNIGHT;

	  }

	  if (squares[sq-d-d] == BORDER && squares[sq+d+d] == EMPTY) {
	    add_normal_move(ml, sq, sq+d+d);  // 2 step move
	  }
	}
	break;

      case KNIGHT:
	append_knight_pseudo_moves(col, sq, ml);
	break;

      case BISHOP:
	append_bishop_pseudo_moves(col, sq, ml);
	break;
	
      case ROOK:
	append_rook_pseudo_moves(col, sq, ml);
	break;

      case QUEEN:
	append_queen_pseudo_moves(col, sq, ml);
	break;

      case KING: 

	moves_on_circle(col, ml, sq, KING_D);

	assert((BoardType::board_width(type) & 1) == 0);
	
	if (col == WHITE)
	  kl = ll_sq() + BoardType::board_width(type)/2;
	else
	  kl = ul_sq() + BoardType::board_width(type)/2;

	if (sq == kl) {

	  sint4 kx, ky;
	  Move::ind_to_xy(kl, kx, ky);
	  
	  if (CK[col]) {

	    // king side castling
	    // squares between rook and king empty?

	    sint4 loc = kl+E;

	    while (squares[loc] == EMPTY) loc += E;

	    if (abs(squares[loc]) == ROOK && squares[loc+E] == BORDER) {

	      bool a[2], b[2];

	      attacked(kl, a);
	      attacked(kl+E, b);

	      if (!a[opponent(col)] && !b[opponent(col)]) {
		add_normal_move(ml, sq, sq+E+E);  // castling
	      }
	    }
	  }
	
	  if (CQ[col]) {

	    sint4 loc = kl+W;

	    while (squares[loc] == EMPTY) loc += W;
	    
	    if (abs(squares[loc]) == ROOK && squares[loc+W] == BORDER) {

	      bool a[2], b[2];

	      attacked(kl, a);
	      attacked(kl+W, b);
	      
	      if (!a[opponent(col)] && !b[opponent(col)]) {
		add_normal_move(ml, sq, sq+W+W);
	      }
	    }
	  }
	}
	break;

      default:
	assert(0);
      }
    }
  }
}


void Board::append_knight_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const
{
  moves_on_circle(col, ml, sq, KNIGHT_D);
}

void Board::append_bishop_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const
{
  moves_on_line(col, ml, sq, NE);
  moves_on_line(col, ml, sq, NW);
  moves_on_line(col, ml, sq, SE);
  moves_on_line(col, ml, sq, SW);
}

void Board::append_rook_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const
{
  moves_on_line(col, ml, sq, N);
  moves_on_line(col, ml, sq, S);
  moves_on_line(col, ml, sq, W);
  moves_on_line(col, ml, sq, E);
}

void Board::append_queen_pseudo_moves(sint4 col, sint4 sq, MoveList &ml) const
{
  moves_on_line(col, ml, sq, NE);
  moves_on_line(col, ml, sq, NW);
  moves_on_line(col, ml, sq, SE);
  moves_on_line(col, ml, sq, SW);
  moves_on_line(col, ml, sq, N);
  moves_on_line(col, ml, sq, S);
  moves_on_line(col, ml, sq, W);
  moves_on_line(col, ml, sq, E);
}
  
void Board::legal_moves(sint4 col, MoveList &ml) const
{
  MoveList pml;

  pseudo_legal_moves(col, pml);

  ml.erase(ml.begin(), ml.end());

  FORU (i, pml.size()) {
    Board copy = *this;
    copy.turn_color = col;
    copy.do_move(pml[i]);
    if (!copy.in_check(col)) ml.push_back(pml[i]);
  }
}

bool Board::legal_move(const Move &mv)
{
  MoveList ml;
  
  legal_moves(turn_color, ml);
  
  uint4 i;

  // FOR (i, ml.size()) { errsttr << i << " "; ml[i].write(errsttr); errsttr << endl; }
  // errsttr << "MOVE="; mv.write(errsttr); errsttr << endl;


  FOR (i, ml.size()) if (mv == ml[i]) break;
  return i < ml.size();
}



// move must be legal!

void Board::do_move(const Move &move)
{
  sint4  from, to;
  sint4 col;
  sint4 moved_piece;

  from = move.from;
  to   = move.to;

  moved_piece = squares[from];
   
  assert(abs(moved_piece) > 0);

  col = color_of(moved_piece);

// errsttr << int(col) << int(to_move) << "XXXXX" << endl << flush;

  assert(col == turn_color);

  sint4 victim = squares[to];

  if (victim != EMPTY && color_of(victim) == turn_color) victim = EMPTY;
  
  if (abs(victim) > 0 || abs(squares[from]) == PAWN) {

    // irreversible move

    rev_move_num = 0;

  } else {

    // reversible move
    
    ++rev_move_num;
  }
      
  // check if castable rook is captured: update castling status

  // if (victim != EMPTY) errsttr << "victim=" << victim << endl;
      
      
  if (abs(victim) == ROOK) {

    // errsttr << "victim ROOK" << endl;
    
    if (victim == W_ROOK) {
      if      (to == ll_sq()) CQ[WHITE] = false;
      else if (to == lr_sq()) CK[WHITE] = false;
    } else {
      if      (to == ul_sq()) CQ[BLACK] = false;
      else if (to == ur_sq()) CK[BLACK] = false;
    }
  }

  if (abs(moved_piece) == KING && abs(from-to) == 2) {

    // castling

    if (to > from) {
      assert(abs(squares[r_sq(col)]) == Board::ROOK);
      squares[r_sq(col)] = EMPTY;
      squares[to-1] = make_cont(ROOK, col);
    } else {
      assert(abs(squares[l_sq(col)]) == Board::ROOK);
      squares[l_sq(col)] = EMPTY;
      squares[to+1] = make_cont(ROOK, col);
    }
  }

  if (abs(moved_piece) == PAWN && to == ep_square) {

    // en-passant

    squares[to - UP_ONE_RANK_FOR_COLOR[col]] = EMPTY;
  }

  squares[to] = squares[from];
  squares[from] = EMPTY;
  ep_square = -1;

  switch (move.prom) {

  case Move::NO_PROM:

    switch (abs(moved_piece)) {

    case PAWN:

      if (abs(to-from) == TWO_RANKS) 
	ep_square = (to+from) / 2;

      break;

    case ROOK:

      if (col == WHITE) {
	
	if (from == ll_sq()) { 
	  CQ[col] = false;
	} else if (from == lr_sq()) { 
	  CK[col] = false;
	}

      } else { 

	if (from == ul_sq()) { 
	  CQ[col] = false;
	} else if (from == ur_sq()) { 
	  CK[col] = false;
	}
      }
      break;

    case KING:
      CQ[col] = false;
      CK[col] = false;
      break;

    default: ;
    }
    break;

  case Move::PROM_TO_KNIGHT:
    assert(abs(moved_piece) == Board::PAWN);
    squares[to] = make_cont(KNIGHT, col);
    break;

  case Move::PROM_TO_BISHOP:
    assert(abs(moved_piece) == Board::PAWN);
    squares[to] = make_cont(BISHOP, col);
    break;

  case Move::PROM_TO_ROOK:
    assert(abs(moved_piece) == Board::PAWN);
    squares[to] = make_cont(ROOK, col);
    break;

  case Move::PROM_TO_QUEEN:
    assert(abs(moved_piece) == Board::PAWN);
    squares[to] = make_cont(QUEEN, col);
    break;

  default:
    assert(0);
  }

  // finally toggle side to move

  toggle_to_move();
}


sint4 Board::king_loc(sint4 col) const
{
  sint4 pc;

  pc = KING;
  if (col == BLACK) pc = -pc;
  FORS (i, (sint4)BoardType::MAX_BOARD_SIZE) if (squares[i] == pc) return i;
  return -1;
}


sint4 Board::scan_line(sint4 sq, sint4 d) const
{
  sint4 i = sq;
  
  FOREVER { i += d; if (squares[i] != EMPTY) break; }
  return i;
}


void Board::attacked(sint4 sq, bool a[2]) const
{
  sint4 pc, cont, psq;

  a[0] = a[1] = false;

  // -- |

  FORS (i, 4) { 
    psq = scan_line(sq, HORI_VERT_D[i]);
    cont = squares[psq]; pc = abs(cont);
    if (pc >= PAWN) {
      if (pc == ROOK || 
	  pc == QUEEN ||
          (pc == KING && psq == sq + HORI_VERT_D[i])) {

	a[color_of(cont)] = true;
	if (a[0] && a[1]) return;
      }
    }
  }

  // 

  FORS (i, 4) { 
    psq = scan_line(sq, DIAG_D[i]);
    cont = squares[psq]; pc = abs(cont);
    if (pc >= PAWN) {
      if (pc == BISHOP || 
          pc == QUEEN  ||
          (pc == KING && psq == sq + DIAG_D[i]) ||
          (pc == PAWN && psq == sq + DIAG_D[i] && 
	   ((cont < 0 && DIAG_D[i] > 0) ||
	    ((cont > 0 && DIAG_D[i] < 0))))) {
	a[color_of(cont)] = true;
	if (a[0] && a[1]) return;
      }
    }
  }
  
  // knights

  FORS (i, 8) { 
    cont = squares[sq + KNIGHT_D[i]];
    if (abs(cont) == KNIGHT) {
      a[color_of(cont)] = true;
      if (a[0] && a[1]) return;
    }
  }
}


bool Board::has_color(sint4 cf, sint4 col)
{
  assert(abs(cf) > 0);
  if (col == WHITE) return cf > 0; else return cf < 0;
}


sint4 Board::color_of(sint4 sc)
{
  assert(abs(sc) > 0);
  if (sc > 0) return WHITE; else return BLACK;
}


sint4 Board::man_of(sint4 sc)
{
  if (sc == EMPTY) return NONE;
  assert(abs(sc) > 0);
  assert(1 == PAWN);
  return abs(sc);
}


bool Board::is_man(sint4 sc)
{
  return sc != EMPTY && sc != BORDER;
}


Board::SqCont Board::make_cont(sint4 pc, sint4 col)
{
  if (pc == NONE) return EMPTY;
  assert(PAWN == 1);
  sint4 sc = pc;
  if (col == WHITE) return SqCont(sc); else return SqCont(-sc);
}


sint4 Board::ll_sq() const { return Move::xy_to_ind(0, 0); }

sint4 Board::lr_sq() const {
  sint4 w = BoardType::board_width(type);
  return Move::xy_to_ind(w-1, 0);
}

sint4 Board::ul_sq() const {
  sint4 w = BoardType::board_width(type);
  return Move::xy_to_ind(0, w-1);
}

sint4 Board::ur_sq() const {
  sint4 w = BoardType::board_width(type);
  return Move::xy_to_ind(w-1, w-1);
}

sint4 Board::r_sq(sint4 col) const {
  if (col == WHITE) return lr_sq(); else return ur_sq();
}
  
sint4 Board::l_sq(sint4 col) const {
  if (col == WHITE) return ll_sq(); else return ul_sq();
}
  

/* ADLER32 (Adler-32 checksum)
This contains a checksum value of the uncompressed data
(excluding any dictionary data) computed according to Adler-32
algorithm. This algorithm is a 32-bit extension and improvement
of the Fletcher algorithm, used in the ITU-T X.224 / ISO 8073
standard. See references [4] and [5] in Chapter 3, below)

Adler-32 is composed of two sums accumulated per byte: s1 is
the sum of all bytes, s2 is the sum of all s1 values. Both sums
are done modulo 65521. s1 is initialized to 1, s2 to zero.  The
Adler-32 checksum is stored as s2*65536 + s1 in most-
significant-byte first (network) order.
*/

static uint4 adler32(const vector<uint1> &v)
{
  const uint4 BASE = 65521;
  uint4 s1 = 1, s2 = 0;
  uint4 n = v.size();

  FORU (i, n) {
    s1 += v[i]; if (s1 >= BASE) s1 -= BASE;
    s2 += s1;   if (s2 >= BASE) s2 -= BASE;
  }
  return (s2 << 16) + s1;
}


uint4 Board::adler32() const
{
  vector<uint1> v;

  FORS (i, sint4(BoardType::MAX_BOARD_SIZE)) v.push_back(squares[i]);
  v.push_back(turn_color);
  v.push_back(ep_square);
  FORS (i, 2) { v.push_back(CK[i]); v.push_back(CQ[i]); }

  return ::adler32(v);
}
