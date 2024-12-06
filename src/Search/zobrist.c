#include "zobrist.h"
#include "stdlib.h"

#include "../Core/bitboard.h"

u64 zobristTable[12][64];// the 13th entry is for en passan targets
u64 zobristEnPassanTarget[64];// the 13th entry is for en passan targets
u64 zobristSideToMove;
u64 zobristCastlingRights[4];

static ttEntry* transpositionTable;
static int tableSize;

static u64 randu64(){
  u64 u1, u2, u3, u4;
  u1 = (u64)(random()) & 0xFFFF; u2 = (u64)(random()) & 0xFFFF;
  u3 = (u64)(random()) & 0xFFFF; u4 = (u64)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

void initTranspositionTable(int bytes){
  tableSize = (bytes/sizeof(ttEntry));
  transpositionTable = calloc(tableSize,sizeof(ttEntry));

  for(int i = 0; i<13; i++){
    for(int j = 0; j<64; j++){
      zobristTable[i][j] = randu64();
    }
  }
  zobristSideToMove = randu64();
  for(int i = 0; i<4; i++){
    zobristCastlingRights[i] = randu64();
  }
}

void cleanupTranspositionTable(){
  free(transpositionTable);
}

bool validateZobrist(Board board){
  u64 old = board.zobrist;
  genZobristKey(&board);
  return old == board.zobrist;
}
void genZobristKey(Board *board){
  board->zobrist = 0;
  for(int piece = 0; piece<12; piece++){
    u64 bb = board->bitboards[piece];
    while(bb){
      board->zobrist ^= zobristTable[piece][popls1b(&bb)];
    }
  }
  if(getSideToMove(board) == WHITE) board->zobrist ^= zobristSideToMove;
  board->zobrist ^= zobristEnPassanTarget[board->enPassanTarget];

  if(board->flags&WHITE_KINGSIDE_BIT) board->zobrist ^= zobristCastlingRights[0];
  if(board->flags&WHITE_QUEENSIDE_BIT) board->zobrist ^= zobristCastlingRights[1];
  if(board->flags&BLACK_KINGSIDE_BIT) board->zobrist ^= zobristCastlingRights[2];
  if(board->flags&BLACK_QUEENSIDE_BIT) board->zobrist ^= zobristCastlingRights[3];
}

void transpositionWrite(ttEntry *entry){
  transpositionTable[entry->key%tableSize] = *entry;
}

ttEntry* transpositionLookup(u64 key){
  ttEntry *entry = &transpositionTable[key%tableSize];
  if(entry->key != key) return NULL;
  return entry;
}
