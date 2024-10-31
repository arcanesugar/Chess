#include "eval.h"

#include "../Core/types.h"
#include "../Core/bitboard.h"

int pieceSquareTables[12][64];
void initpsqt(){
  int pieceValues[6] = {
    100,//Pawn
    300,//Bishop
    300,//Knight
    500,//Rook
    900 //Queen
  };
  for(int piece = 0; piece<12; piece++){
    for(int square = 0; square<64; square++){
      pieceSquareTables[piece][square] = pieceValues[piece%6];
    }
  }
  
  //make black piece square tables negative
  for(int piece = 6; piece<12; piece++){
    for(int square = 0; square<64; square++){
      pieceSquareTables[piece][square] = -pieceSquareTables[piece][square];
    }
  }
}

void initEval(){
  initpsqt();
}

double psqt(Board *board){
  double psqt = 0;
  for(int piece = 0; piece<12; piece++){
    u64 bb = board->bitboards[piece];
    while(bb){
      psqt += pieceSquareTables[piece][popls1b(&bb)];
    }
  }
  return psqt;
}

//a positive evaluation means the position is good for white
double evaluate(Board *board){
  double evaluation = 0.0;
  evaluation += psqt(board);
  return evaluation;
}