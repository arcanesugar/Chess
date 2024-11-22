#pragma once
#include "../Core/board.h"
#include "../Core/move.h"

typedef struct ttEntry{
  u64 key;
  int eval;
  int depth;
  int type;
}ttEntry;

extern u64 zobristTable[12][64];// the 13th entry is for en passan targets
extern u64 zobristEnPassanTarget[64];// the 13th entry is for en passan targets
extern u64 zobristSideToMove;
extern u64 zobristCastlingRights[4];

bool validateZobrist(Board board);
void initTranspositionTable(int bytes);
void cleanupTranspositionTable();
void genZobristKey(Board *board);
ttEntry* transpositionLookup(u64 key);
void transpositionWrite(ttEntry *entry);
