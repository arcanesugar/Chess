#include "eval.h"

double material(Board &board){
  double material = 0.0;
  double values[5] = {1,3,3,5,9};
  for(int i = 0; i<KING; i++){
    material += (bitcount(board.bitboards[WHITE+i]) - bitcount(board.bitboards[BLACK+i]))*values[i];
  }
  return material;
}

//a positive evaluation means the position is good for the current player
double evaluate(Board &board){
  double evaluation = 0.0;
  if(board.flags & WHITE_TO_MOVE_BIT){
    evaluation += material(board);
  }else{
    evaluation -= material(board);
  }
  return evaluation;
}