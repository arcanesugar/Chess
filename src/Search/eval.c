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
    int bonus = squareValues[square];
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
  int mobilityValue = 0;
  for(int piece = 0; piece<12; piece++){
    for(int square = 0; square<64; square++){
      pieceSquareTables[piece][square] = pieceValues[piece%6];
    }
  }
  int objSquareVal[64] = {//being able to move to these squares is good
    1,1,1,1,1,1,1,1,
    1,2,2,2,2,2,2,1,
    1,2,3,3,3,3,3,1,
    1,2,3,4,4,3,3,1,
    1,2,3,4,4,3,3,1,
    1,2,3,3,3,3,3,1,
    1,2,2,2,2,2,2,1,
    1,1,1,1,1,1,1,1,
  };
  int pawnTable[64] = {
    0,0,0,0,0,0,0,0,//a pawn will never actually reach these squares because they get promoted
    9,9,9,9,9,9,9,9,
    0,0,0,0,0,0,0,0,
    0,0,0,4,4,0,0,0,
    0,0,0,5,5,0,0,0,
    3,0,0,1,1,0,0,3,//letting the pawn move up a square helps not suffocate the king, so this is probably a good idea
    3,3,3,-1,-1,3,3,3,
    0,0,0,0,0,0,0,0
  };
  for(int i = 0; i<64; i++){
    pieceSquareTables[PAWN][i] += pawnTable[63-i]*10;//64-i makes the squares actually correct
  }
  u64 queenMasks[64];
  for(int square = 0; square<64; square++) queenMasks[square] = bishopMasks[square] | rookMasks[square];

  //Giving the queen a mobility bonus makes it more agressive
  psqtMobility(pieceSquareTables[KNIGHT], knightMoves, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[BISHOP], bishopMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[ROOK], rookMasks, objSquareVal, mobilityValue);
  psqtMobility(pieceSquareTables[QUEEN], queenMasks, objSquareVal, mobilityValue);
  
  //Create black peice square tables by negating the white ones and flipping them over the y axis;
  for(int piece = 6; piece<12; piece++){
    for(int square = 0; square<64; square++){
      int sx = square%8;
      int sy = square/8;
      sy = 7-sy;
      pieceSquareTables[piece][(sy*8)+sx] = -pieceSquareTables[piece-BLACK][square];
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
  if(board->flags&WHITE_TO_MOVE_BIT) return evaluation;
  return -evaluation;
}
