#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#include "../Board/board.h"
#include "Magic/magic.h"

typedef struct MoveList MoveList;

extern u64 knightMoves[64];
extern u64 kingMoves[64];


//initialisation functions
void createKnightTable();//fills the knight moves array, does not do actual move generation
void createKingTable();//see above comment

void addMovesToSquares(MoveList *moves, int fromSquare, u64 squares);
void addMovesFromOffset(MoveList *moves, int offset, u64 targets, byte flags);

void addDiagonalMoves(Board *board, int square, MoveList *moves);
void addOrthogonalMoves(Board *board, int square, MoveList *moves);

void addSlidingMoves(Board *board, MoveList *moves);
void addPawnMoves(Board *board, MoveList *moves);
void addKnightMoves(Board *board, MoveList *moves);
void addKingMoves(Board *board, MoveList *moves);
void addCastlingMoves(Board *board, MoveList *moves);

bool isAttacked(Board *board, byte square, byte opponentColor);
void filterLegalMoves(Board *board, MoveList *moves);

//public:
void initMoveGenerator();
void cleanupMoveGenerator();
void generateMoves(Board *board, MoveList *moves);

#ifdef __cplusplus
}
#endif
