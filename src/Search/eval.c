#include "eval.h"

#include "../Core/types.h"
#include "../Core/bitboard.h"
#include "../Movegen/movegen.h"
#include "../Movegen/Magic/magic.h"

int pieceSquareTables[12][64];
static int endgameKingTables[2][64];
static const int pieceValues[6] = {
  100,//Pawn
  300,//Bishop
  300,//Knight
  500,//Rook
  900 //Queen
};
static void psqtMobility(int *psqt, u64 *moveMasks, const int* squareValues, int value){
  int bonuses[64];
  int maxBonus = 0;
  for(int square = 0; square<64; square++){
    u64 mask = moveMasks[square];
    int bonus = squareValues[square];
    while(mask){
      bonus += squareValues[popls1b(&mask)];
    }
    bonuses[square] = bonus;
    if(bonus>maxBonus) maxBonus = bonus;
  }
  if(maxBonus == 0) return;
  for(int square = 0; square<64; square++){
    psqt[square] += (float)value*((float)bonuses[square]/(float)maxBonus);
  }
}

void initpsqt(){

  int mobilityValue = 50;
  for(int piece = 0; piece<12; piece++){
    for(int square = 0; square<64; square++){
      pieceSquareTables[piece][square] = pieceValues[piece%6];
    }
  }

  const int movesToEdge[64] = {
    0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,
    0,1,2,2,2,2,1,0,
    0,1,2,3,3,2,1,0,
    0,1,2,3,3,2,1,0,
    0,1,2,2,2,2,1,0,
    0,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,
  };

  int pawnTable[64] = {
    0,0,0,0,0,0,0,0,//a pawn will never actually reach these squares because they get promoted
    9,9,9,9,9,9,9,9,
    0,0,0,0,0,0,0,0,
    0,0,0,4,4,0,0,0,
    0,0,0,5,5,0,0,0,
    3,0,0,0,0,0,0,3,//letting the pawn move up a square helps not suffocate the king, so this is probably a good idea
    3,3,3,-1,-1,3,3,3,//keeping the two center pawns close to the king is penalized
    0,0,0,0,0,0,0,0
  };
  int kingEarly[64] = {
    -3,-4,-4,-5,-5,-4,-4,-3,
    -3,-4,-4,-5,-5,-4,-4,-3,
    -3,-4,-4,-5,-5,-4,-4,-3,
    -3,-4,-4,-4,-4,-4,-4,-4,//being up here is bad probably
    -3,-3,-3,-3,-3,-3,-3,-3,
     0,-1,-3,-3,-3,-3,-1, 0,
     4, 0, 0, 0, 0, 0, 0, 4,
     4, 6, 0,-3,-3, 0, 6, 4 
  };
  for(int i = 0; i<64; i++){
    //64-i makes the squares indices correct
    pieceSquareTables[PAWN][i] += pawnTable[63-i]*10;
    pieceSquareTables[KING][i] += kingEarly[63-i]*10;
    endgameKingTables[0][i] = movesToEdge[i];
    endgameKingTables[1][i] = -movesToEdge[i];
  }
  //using the moves to egde as square values prioritises having pieces in the middle,
  //which is good because the middle of the board is right in the center
  u64 queenMasks[64];
  for(int i = 0; i<64; i++) queenMasks[i] = rookMasks[i] | bishopMasks[i];
  psqtMobility(pieceSquareTables[KNIGHT], knightMoves, movesToEdge, mobilityValue);
  psqtMobility(pieceSquareTables[ROOK],   rookMasks,   movesToEdge, mobilityValue);
  psqtMobility(pieceSquareTables[BISHOP], bishopMasks, movesToEdge, mobilityValue);
  psqtMobility(pieceSquareTables[QUEEN],  queenMasks,  movesToEdge, 20);
  
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

static int psqt(Board *board){
  int psqt = 0;
  int valueOnBoard = 0;
  for(int piece = 0; piece<12; piece++){
    if(piece == KING || piece == BLACK + KING) continue;
    u64 bb = board->bitboards[piece];

    if(piece%6 != PAWN) valueOnBoard += pieceValues[piece%6]*bitcount(bb);
    while(bb){
      psqt += pieceSquareTables[piece][popls1b(&bb)];
    }
  }
  if(valueOnBoard<=2000){
    psqt += endgameKingTables[0][bitScanForward(board->bitboards[KING])];
    psqt += endgameKingTables[1][bitScanForward(board->bitboards[BLACK+KING])];
  }else{
    psqt += pieceSquareTables[KING][bitScanForward(board->bitboards[KING])];
    psqt += pieceSquareTables[BLACK+KING][bitScanForward(board->bitboards[BLACK+KING])];
  }
  return psqt;
}

//a positive evaluation means the position is good for white
int evaluate(Board *board){
  int evaluation = 0.0;
  evaluation += psqt(board);

  if(board->flags&WHITE_TO_MOVE_BIT) return evaluation;
  return -evaluation;
}
