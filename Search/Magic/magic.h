#pragma once
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <unordered_set>
#include "../../types.h"
#include "../../Board/board.h"

struct Magic{
  u64 magic;
  int shift;
  int max;
};

//the actual magic numbers
extern Magic rookMagics[64];
extern Magic bishopMagics[64];
extern u64 *rookMoves[64];//key, moves bitboard 
extern u64 *bishopMoves[64];//key, moves bitboard 
extern u64 rookMasks[64];
extern u64 bishopMasks[64];

//initialisation/cleanup
void initMagics();  
void cleanupMagics();
void saveMagics();
int  loadMagics();

//using magic numbers
inline u64 magicHash(Magic magic, u64 blockers);
u64 rookLookup(u64 blockers, byte rookSquare);
u64 bishopLookup(u64 blockers, byte bishopSquare);

//generating magic numbers

void searchForMagics();
void magicSearch();
void generateRookBlockers();
void generateBishopBlockers();

void generateRookMasks();
void generateBishopMasks();
void fillRookMoves();
void fillBishopMoves();
