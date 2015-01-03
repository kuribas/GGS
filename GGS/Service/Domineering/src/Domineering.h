// $Id: Domineering.h 160 2007-06-22 15:21:10Z mburo $
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
// Domineering.h: interface for the Domineering class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DOMINEERING_H
#define DOMINEERING_H 1

#include <iostream>
#include <string>
#include "RegularBoardGame.H"

using std::ostream;
using std::istream;
using std::string;

class BoardType {

public:

  enum {
    MIN_BOARD_WIDTH = 4,
    MAX_BOARD_WIDTH = 26,
    DX = MAX_BOARD_WIDTH+2,
    MAX_BOARD_SIZE = DX * DX,
    MAX_MOVE_NUM   = MAX_BOARD_SIZE
  };

  static bool legal_rand_type(int bt, int rand_type);

  static int board_width(int bt);
  static int board_squares(int bt);
};

class Domineering : public RegularBoardGame  
{
public:
	void do_move(const string &move_str);
	bool legal_move( ostream &err, const string &move_str);
	int max_result() const;
	int blacks_result(bool anti) const;
	bool game_finished() const;
	void write_pos_txt(ostream &os) const;
	void write_pos_ggf(ostream &os, bool one_line=true) const;
	TURN color_to_move() const;
	bool read_pos_ggf(istream &is);
	void init_pos(int board_type, int rand_type);

	Domineering();
	virtual ~Domineering();

private:
	void ind_to_xy(int ind, int& x, int& y) const;
	void add_random(int r);
	int xy_to_ind(int x, int y) const;
	int type, black_moves, white_moves;
	TURN turn;
	enum SQUARE { BLACK, WHITE, BORDER, EMPTY, UNDEF };
	SQUARE squares[BoardType::MAX_BOARD_SIZE];
	char cont_to_char(SQUARE s) const;
	SQUARE char_to_cont(char c) const;
	int sq_cont(SQUARE s) const;
	int available_moves(SQUARE color) const;
	int available_anti_moves(SQUARE color) const;
};

#endif
