#pragma once
#include <stdio.h>
#include <fstream>
#include <vector> 
#include <unordered_set>
#include <thread>
#include <climits>
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

#define ROOK_BLOCKERS_PER_SQUARE 16384

extern u64 rookBlockers[64][ROOK_BLOCKERS_PER_SQUARE];
extern std::vector<u64> bishopBlockers[64];


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
bool testMagic(std::vector<u64> *blockers, Magic &magic);
void magicSearch();
void generateBlockersFromMask(u64 mask,std::vector<u64> &target);
void generateRookBlockers();
void generateBishopBlockers();

void generateRookMasks();
void generateBishopMasks();
void fillRookMoves();
void fillBishopMoves();
