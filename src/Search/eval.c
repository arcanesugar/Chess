#include "eval.h"

#include "../Core/types.h"
#include "../Core/bitboard.h"
#include "../Movegen/movegen.h"
#include "../Movegen/Magic/magic.h"

int pieceSquareTables[12][64];

void psqtMobility(int *psqt, u64 *moveMasks, int *squareValues, int value){
  int bonuses[64];
  int maxBonus = 0;
  for(int square = 0; square<64; square++){
    u64 mask = moveMasks[square];
    int bonus = 0;
    while(mask){
      bonus+=squareValues[popls1b(&mask)];
    }
    if(bonus>maxBonus) maxBonus = bonus;
    bonuses[square] = bonus;
  }
  for(int square = 0; square<64; square++){
    psqt[square] += value*((float)bonuses[square]/(float)maxBonus);
  }
}
void initpsqt(){
  int pieceValues[6] = {
    100,//Pawn
    300,//Bishop
    300,//Knight
    500,//Rook
    900 //Queen
  };
  int mobilityValue = 100;
  for(int piece = 0; piece<12; piece++){
    for(int square = 0; square<64; square++){
      pieceSquareTables[piece][square] = pieceValues[piece%6];
    }
  }
  int objSquareVal[64] = {//being able to move to these squares is good
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,2,2,2,2,1,1,
    1,1,2,3,3,2,1,1,
    1,1,2,3,3,2,1,1,
    1,1,2,2,2,2,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
  };
  u64 queenMasks[64];
  for(int square = 0; square<64; square++) queenMasks[square] = bishopMasks[square] | rookMasks[square];

  //Giving the queen a mobility bonus makes it more agressive
  psqtMobility(pieceSquareTables[KNIGHT], knightMoves, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[BISHOP], bishopMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[ROOK], rookMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[QUEEN], queenMasks, objSquareVal, mobilityValue);

  psqtMobility(pieceSquareTables[BLACK+KNIGHT], knightMoves, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[BLACK+BISHOP], bishopMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[BLACK+ROOK], rookMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[BLACK+QUEEN], queenMasks, objSquareVal, mobilityValue); 

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
