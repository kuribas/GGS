// $Id: GetMove.C 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.


#include "types.h"

#include "GetMove.h"
#include "OsObjects.h"
#include "AmsObjects.h"

#include <sstream>
using namespace std;

int Evaluate(const COsBoard& board, bool fAnti);

int Evaluate(const COsBoard& board, bool fAnti) {
  int net=board.PieceCount(COsBoard::BLACK)-board.PieceCount(COsBoard::WHITE);
  return ((board.iMover!=0)^fAnti)?-net:net;
}

void GetMove(const COsGame& game, CSGMoveListItem& mli) {
  int x, y;
  int eval, evalSubnode;
  int nWidth=game.mt.bt.nWidth;
  int nHeight=game.mt.bt.nHeight;

  COsBoard board,boardSub;
  bool fAnti=game.mt.fAnti;

  eval=-1000;
  mli.mv="PA";
  mli.dEval=0;	// return eval of 0 if we are forced to pass;

  board=game.pos.board;

  for (x=0; x<nWidth; x++) {
    for (y=0; y<nHeight; y++) {
      if (board.IsMoveLegal(x,y)) {

	boardSub=board;
	boardSub.UpdateXY(x,y);
	evalSubnode=Evaluate(boardSub, fAnti);
	cerr << CSGSquare(x,y).StringGet() << " " << evalSubnode << "\n";
	if (evalSubnode>eval) {
	  eval=evalSubnode;
	  mli.mv=CSGSquare(x,y).StringGet();
	  mli.dEval=eval;
	}
      }
    }
  }
}

void GetMove(const CCksGame& game, CSGMoveListItem& mli) {
  CSGSquare sq;
  bool fJump;
  CCksBoard board=game.pos.board;

  vector<CSGSquare> sqs=board.GetStartSquares(board.iMover, fJump);

  // we just take the first legal move returned.
  QSSERT(!sqs.empty());
  sq=sqs[0];
  mli.mv=sq.StringGet();

  if (fJump) {
    while (!(sqs=board.GetJumpEndSquares(sq)).empty()) {
      CSGSquare sqEnd=sqs[0];
      mli.mv+='-';
      mli.mv+=sqEnd.StringGet();
      board.UpdatePiece(sq, sqEnd);
      sq=sqEnd;
    }
  }
  else {
    sqs=board.GetSlideEndSquares(sq);
    QSSERT(!sqs.empty());
    sq=sqs[0];
    mli.mv+='-';
    mli.mv+=sq.StringGet();
  }
}

void GetMove(const CAmsGame& /*game*/, CSGMoveListItem& /*mli*/) {
  // no amazons GetMove function, you need to supply your own
}
