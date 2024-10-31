#pragma once

#include "../../Core/types.h"

struct Magic{
  u64 magic;
  int shift;
  int max;
};
typedef struct Magic Magic;

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
