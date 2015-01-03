// $Id: Domineering.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    Copyright 2002 Daniel Lidström, danli97@ite.mh.se

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

// Domineering.cpp: implementation of the Domineering class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <ctype.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <string>
#include <iomanip>
#include <sstream>
#include "Domineering.h"

using std::setw;
using std::string;
using std::random_shuffle;
using std::copy;
using std::cerr;
using std::ostream_iterator;
using std::stringstream;
using std::vector;
using std::cout;
using std::endl;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Domineering::Domineering()
{
	for( int i=0; i<BoardType::MAX_BOARD_SIZE; i++ )
		squares[i] = BORDER;
	srand(time(0));
	black_moves = white_moves = 0;
}

Domineering::~Domineering()
{

}

// false iff move_str is illegal in POS (write error message to err),
bool Domineering::legal_move( ostream &err, const string &move_str)
{
	if(move_str.size() < 2) { err << "illegal move"; return false; }

	stringstream stream(move_str);

  int x = toupper(stream.get()) - 'A' + 1;

	// check x first
	int w = BoardType::board_width(type);
	if( x<1 || x>w ) {
		err << "move-x out of range" << EOL;
		return false;
	}

	int y;
	stream >> y;

	if(!stream) {
		err << "missing or illegal move-y" << EOL;
		return false;
	}

	// check row next
	if( y>w || y<1 ) {
		err << "move-y out of range" << EOL;
		return false;
	}

	// check if we have trailing characters
	char c;
	stream >> c;
	
	if( !stream.eof() ) {
		err << "illegal move '" << c << "'"; return false;
	}

	if( turn == RegularBoardGame::BLACK ) { // vertical
		if( squares[xy_to_ind(x, y)] == EMPTY && squares[xy_to_ind(x, y+1)] == EMPTY )
			return true;
	} else { // horizontal
		if( squares[xy_to_ind(x, y)] == EMPTY && squares[xy_to_ind(x+1, y)] == EMPTY )
			return true;
	}
	err << "bad move" << EOL;
	return false;
}

// init POS w.r.t. board_type and rand_type (<0 => no rand)
void Domineering::init_pos(int board_type, int rand_type)
{
	int x, y;
	type = board_type;

	int w = BoardType::board_width(board_type);
	for( x=1; x<=w; x++ ) {
		for( y=1; y<=w; y++ ) {
			int ind = xy_to_ind(x, y);
			squares[ind] = EMPTY;
		}
	}

	if( rand_type > 0 )
		add_random(rand_type);

	if( sq_cont(BLACK) > sq_cont(WHITE) )
		turn = RegularBoardGame::WHITE;
	else
		turn = RegularBoardGame::BLACK;
}

// init POS from ggf representation
bool Domineering::read_pos_ggf(istream &is)
{
	is >> type;
	if( !is ) {
		cerr << "bad type" << endl;
		return false;
	}

	char c;
	SQUARE s;
	int w = BoardType::board_width(type);
	for( int y=1; y<=w; y++ ) {
		for( int x=1; x<=w; x++ ) {
			int ind = xy_to_ind(x, y);
			is >> c;
			s = char_to_cont(c);
			if( s == UNDEF )
				cerr << "bad square content: '" << c <<'\'' << endl;
			else
				squares[ind] = s;
		}
	}

	is >> c;
	s = char_to_cont(c);
	if( s==UNDEF )
		cerr << "bad turn to move: " << c << endl;
	else
		turn = s==WHITE ? RegularBoardGame::WHITE : RegularBoardGame::BLACK;

	return true;
}

// color to move in POS (BLACK | WHITE)
Domineering::TURN Domineering::color_to_move() const
{
	return turn;
}

// write POS in ggf format, use only one line if flag is set
void Domineering::write_pos_ggf(ostream &os, bool one_line) const
{
	os << type;
	if( one_line ) os << ' ';
	else           os << EOL;

	int w = BoardType::board_width(type);
	for( int y=1; y<=w; y++ ) {
		for( int x=1; x<=w; x++ ) {
			char c = cont_to_char(squares[xy_to_ind(x,y)]);
			os << c;
		}
		if( one_line ) os << ' ';
		else           os << EOL;
	}
	if( turn == RegularBoardGame::BLACK ) os << cont_to_char(BLACK);
	else                                  os << cont_to_char(WHITE);
}

// write POS in (verbose) text format
//    A B C D
//   +-+-+-+-+
// 1 |W|W| | |
//   +-+-+-+-+
// 2 |W|W|W|W|
//   +-+-+-+-+
// 3 | |B| |B|
//   +-+-+-+-+
// 4 | |B| |B|
//   +-+-+-+-+
//
// Turn: Vertical
void Domineering::write_pos_txt(ostream &os) const
{
	int w = BoardType::board_width(type), x, y, i;

	os << "   ";
	for( i=0; i<w; i++ ) os << ' ' << char('A'+i);
	os << EOL;
	os << "   +";
	for( i=0; i<w; i++ ) os << "-+";
	os << EOL;

	for( y=1; y<=w; y++ ) {
		os << setw(2) << y << " |";
		for( x=1; x<=w; x++ ) {
			char c = cont_to_char(squares[xy_to_ind(x,y)]);
			os << c << "|";
		}
		os << ' ';
		if( w>=10 )
			os << setw(2) << y;
		else
			os << y;
		os << EOL;
	}

	os << "   +";
	for( i=0; i<w; i++ ) os << "-+";
	os << EOL;
	os << "   ";
	for( i=0; i<w; i++ ) os << ' ' << char('A'+i);
	os << EOL;

	os << cont_to_char(turn==RegularBoardGame::BLACK?BLACK:WHITE) << " to move ";
	if( turn == RegularBoardGame::BLACK ) os << "(vertical)";
	else                                  os << "(horizontal)";
	os << EOL;
}

// true iff game is finished
// algorithm:
// search through the board and try to find
// places where both players can move,
// they look like this:
// +-+-+ +-+-+   +-+ +-+
// | | | | | |   | | | |
// +-+-+ +-+-+ +-+-+ +-+-+
// | |     | | | | | | | |
// +-+     +-+ +-+-+ +-+-+
bool Domineering::game_finished() const
{
	int w = BoardType::board_width(type), x, y;
	// search S-E
	for( x=1; x<=w; x++ ) {
		for( y=1; y<=w; y++ ) {
			int p = xy_to_ind(  x,   y);
			int s = xy_to_ind(  x, y+1);
			int e = xy_to_ind(x+1,   y);
			if( squares[p] == EMPTY && squares[s] == EMPTY && squares[e] == EMPTY )
				return false;
		}
	}
	// search S-W
	for( x=1; x<=w; x++ ) {
		for( y=1; y<=w; y++ ) {
			int p = xy_to_ind(  x,   y);
			int s = xy_to_ind(  x, y+1);
			int w = xy_to_ind(x-1,   y);
			if( squares[p] == EMPTY && squares[s] == EMPTY && squares[w] == EMPTY )
				return false;
		}
	}
	// search N-W
	for( x=1; x<=w; x++ ) {
		for( y=1; y<=w; y++ ) {
			int p = xy_to_ind(  x,   y);
			int n = xy_to_ind(  x, y-1);
			int w = xy_to_ind(x-1,   y);
			if( squares[p] == EMPTY && squares[n] == EMPTY && squares[w] == EMPTY )
				return false;
		}
	}
	// search N-E
	for( x=1; x<=w; x++ ) {
		for( y=1; y<=w; y++ ) {
			int p = xy_to_ind(  x,   y);
			int n = xy_to_ind(  x, y-1);
			int e = xy_to_ind(x+1,   y);
			if( squares[p] == EMPTY && squares[n] == EMPTY && squares[e] == EMPTY )
				return false;
		}
	}

	// found none, game is over
	return true;
}

// game result for black
int Domineering::blacks_result(bool anti) const
{
	int black_moves = this->black_moves;
	int white_moves = this->white_moves;
	if( anti ) {
		black_moves += available_anti_moves(BLACK);
		white_moves += available_anti_moves(WHITE);
	}
	else {
		black_moves += available_moves(BLACK);
		white_moves += available_moves(WHITE);
	}
	return black_moves - white_moves;
}

// maximum result in position POS
// in view of player to move, i.e.
// if white is moving, return available moves
// for white
int Domineering::max_result() const
{
	if( turn == RegularBoardGame::BLACK )
		return available_moves(BLACK);
	else
		return available_moves(WHITE);
}

// make legal move in POS
void Domineering::do_move(const std::string &move_str)
{
	stringstream stream(move_str);

  int x = toupper(stream.get()) - 'A' + 1;
	int y;
	stream >> y;

	if( turn == RegularBoardGame::BLACK ) { // vertical
		squares[ xy_to_ind(x, y) ] = squares[ xy_to_ind(x, y+1) ] = BLACK;
		turn = RegularBoardGame::WHITE;
		black_moves++;
	}
	else { // horizontal
		squares[ xy_to_ind(x, y) ] = squares[ xy_to_ind(x+1, y) ] = WHITE;
		turn = RegularBoardGame::BLACK;
		white_moves++;
	}
}


int Domineering::xy_to_ind(int x, int y) const
{
	return y*BoardType::DX + x;
}

void Domineering::add_random(int r)
{
	vector<int> positions;
	vector<SQUARE> dominoes;
	int i, w, x, y;

  int rnd_white = r / 2;
  if( r%2 && rand()%2 ) rnd_white++; // odd rr, one more white domino 50%

	int rnd_black = r - rnd_white;

	for(i=0; i<rnd_white; i++) dominoes.push_back(WHITE);
	for(i=0; i<rnd_black; i++) dominoes.push_back(BLACK);

	// randomize color sequence

	random_shuffle(dominoes.begin(), dominoes.end());

	w = BoardType::board_width(type);
	for( x=1; x<w; x+=2 ) {
		for( y=1; y<w; y+=2 ) {
			positions.push_back(xy_to_ind(x,y));
		}
	}

	random_shuffle(positions.begin(), positions.end());

	typedef std::vector<SQUARE>::iterator iterator;
	typedef std::vector<int>::iterator iterator2;
	iterator2 pos = positions.begin();
	for( iterator it=dominoes.begin(); it!=dominoes.end(); ++it ) {
		int x, y;
		ind_to_xy(*pos, x, y);
		if( *it == BLACK ) { // vertical
			if( rand() % 2 )
				squares[xy_to_ind(x+1, y)] = squares[xy_to_ind(x+1, y+1)] = BLACK;
			else
				squares[xy_to_ind(x,   y)] = squares[xy_to_ind(  x, y+1)] = BLACK;
		}
		else { // horizontal
			if( rand() % 2 )
				squares[xy_to_ind(x, y+1)] = squares[xy_to_ind(x+1, y+1)] = WHITE;
			else
				squares[xy_to_ind(x,   y)] = squares[xy_to_ind(x+1,   y)] = WHITE;
		}
		++pos;
	}
}

void Domineering::ind_to_xy(int ind, int &x, int &y) const
{
	y = ind/BoardType::DX;
	x = ind%BoardType::DX;
}

Domineering::SQUARE Domineering::char_to_cont(char c) const
{
	SQUARE s;
	switch( c ) {
		case '*': s = BLACK; break;
		case 'O': s = WHITE; break;
		case '-': s = EMPTY; break;
		case 'B': s = BORDER; break;
		default:  s = UNDEF;
	}

	return s;
}

char Domineering::cont_to_char(SQUARE s) const
{
	char c = char();
	switch( s ) {
		case BLACK:  c = '*'; break;
		case WHITE:  c = 'O'; break;
		case EMPTY:  c = '-'; break;
		case BORDER: c = 'B'; break;
		default:     c = -1;
	}

	return c;
}

int Domineering::sq_cont(SQUARE s) const
{
	int num = int();
	for( int i=0; i<BoardType::MAX_BOARD_SIZE; i++ )
		if( squares[i] == s )
			num++;
	return num;
}

//
// scan through and find all places color can move
// this is easier than it sounds, i.e. for horizontal:
// if he can move at a square, move two squares and try again,
// else move one square
//
int Domineering::available_moves(SQUARE color) const
{
	int w = BoardType::board_width(type), x, y;

	int moves = int();

	if( color == WHITE ) { // horizontal
		for( y=1; y<=w; y++ ) {
			for( x=1; x<=w; ) {
				int p = xy_to_ind(  x,   y);
				int e = xy_to_ind(x+1,   y);
				if( squares[p] == EMPTY && squares[e] == EMPTY ) {
					moves ++;
					x+=2;
				}
				else
					x++;
			}
		}
	} else { // vertical
		for( x=1; x<=w; x++) {
			for( y=1; y<=w; ) {
				int p = xy_to_ind(  x,   y);
				int s = xy_to_ind(  x, y+1);
				if( squares[p] == EMPTY && squares[s] == EMPTY ) {
					moves ++;
					y+=2;
				}
				else
					y++;
			}
		}
	}

	return moves;
}

//
// scan through and find all places color can move
// if three-space is found, place on middle, then move
// forward past three-space, if two-space found, place
//
int Domineering::available_anti_moves(SQUARE color) const
{
	int w = BoardType::board_width(type), x, y;

	int moves = int();

	if( color == WHITE ) { // horizontal
		for( y=1; y<=w; y++ ) {
			for( x=1; x<=w; ) {
				int p  = xy_to_ind(  x,   y);
				int e1 = xy_to_ind(x+1,   y);
				int e2 = xy_to_ind(x+1,   y);
				if( squares[p] == EMPTY && squares[e1] == EMPTY && squares[e2] == EMPTY ) {
					moves ++;
					x+=3;
				}
				else if( squares[p] == EMPTY && squares[e1] == EMPTY ) {
					moves ++;
					x+=2;
				}
				else
					x++;
			}
		}
	} else { // vertical
		for( x=1; x<=w; x++) {
			for( y=1; y<=w; ) {
				int p  = xy_to_ind(  x,   y);
				int s1 = xy_to_ind(  x, y+1);
				int s2 = xy_to_ind(  x, y+2);
				if( squares[p] == EMPTY && squares[s1] == EMPTY && squares[s2] == EMPTY ) {
					moves ++;
					y+=3;
				}
				else if( squares[p] == EMPTY && squares[s1] == EMPTY ) {
					moves ++;
					y+=2;
				}
				else
					y++;
			}
		}
	}

	return moves;
}

// static functions

bool BoardType::legal_rand_type(int bt, int rand_type)
{
	int squares = board_squares(bt);
	if( rand_type < 0 || rand_type > (squares/4-1) ) return false;
	return true;
}

int BoardType::board_width(int bt)
{
	if( bt<=MAX_BOARD_WIDTH && bt>=MIN_BOARD_WIDTH && !(bt%2) )
		return bt;

	return -1;
}

int BoardType::board_squares(int bt)
{
	if( bt<=MAX_BOARD_WIDTH && bt>=MIN_BOARD_WIDTH )
		return bt*bt;
	
	return -1;
}


double RegularBoardGame::score(double result) {
	
	const double K = 0.75;
	
	double a = fabs(result);
	
	return 0.5 + result * K * 0.5 / ( 1.0 + a * K  );
}

// read board type, true iff board type is OK, store it as integer
bool RegularBoardGame::read_board_type(istream &is, int &bt) {
	is >> bt;
	if( !is ) return false;
	return BoardType::board_width(bt) > 0;
}

// read rand type, true iff rand_type is OK for board_type, store it as integer
bool RegularBoardGame::read_rand_type(istream &is, int bt, int &rt) {
	is >> rt;
	if (!is) return false;
	return BoardType::legal_rand_type(bt, rt);
}

RegularBoardGame *RegularBoardGame::new_game() { return new Domineering(); }

const int RegularBoardGame::RESULT_INCREMENT = 1;

const bool   RegularBoardGame::HAS_RAND      = true;
const bool   RegularBoardGame::HAS_RAND_TYPE = true;
const bool   RegularBoardGame::HAS_KOMI      = true;
const bool   RegularBoardGame::HAS_ANTI      = true;
const bool   RegularBoardGame::HAS_SYNCHRO   = true;

const char * const RegularBoardGame::GAME_NAME = "Domineering";

const char * const RegularBoardGame::LOGIN_SERVICE = "/dm";
const char * const RegularBoardGame::LOGIN_SYSTEM  = " d.o.m.i.n.e.e.r.i.n.g ";

const char * const RegularBoardGame::DEFAULT_CLOCK = "15:00//";

const char * const RegularBoardGame::FIRST_COLOR  = "vertical";
const char * const RegularBoardGame::SECOND_COLOR = "horizontal";

void RegularBoardGame::gdb() {}

