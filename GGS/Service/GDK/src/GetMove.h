// $Id: GetMove.h 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

// Copyright 2001 Chris Welty
//	All Rights Reserved
// This file is distributed subject to GNU GPL version 2. See the files
// Copying.txt and GPL.txt for details.

#ifndef GDK_GETMOVE_H
#define GDK_GETMOVE_H

#include "types.h"

#include "SGObjects.h"
#include "OsObjects.h"
#include "CksObjects.h"
#include "AmsObjects.h"

void GetMove(const COsGame& game, CSGMoveListItem& mli);
void GetMove(const CCksGame& game, CSGMoveListItem& mli);
void GetMove(const CAmsGame& game, CSGMoveListItem& mli);

#endif	//GDK_GETMOVE_H
