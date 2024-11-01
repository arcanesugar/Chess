#pragma once

#include "../Core/board.h"

typedef struct MoveList MoveList;

extern u64 knightMoves[64];
extern u64 kingMoves[64];

void initMoveGenerator();
void cleanupMoveGenerator();
void generateMoves(Board *board, MoveList *moves);

bool isAttacked(Board *board, byte square, byte opponentColor);
