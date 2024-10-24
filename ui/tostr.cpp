#include "tostr.h"

void moveToString(Move move, char *target){
  if(move.isKingside()){
    strcpy(target, "ks");
    return;
  }
  if(move.isQueenside()){
    strcpy(target, "qs");
    return;
  }
  char fromStr[3] = "  ";
  char toStr[3] = "  ";
  fromStr[0] = 'h'-(move.getFrom()%8);
  toStr[0] = 'h'-(move.getTo()%8);
  fromStr[1] = '1'+(move.getFrom()/8);
  toStr[1] = '1'+(move.getTo()/8);
  
  strcpy(target, "[");
  strcat(target,fromStr);
  strcat(target, "->");
  strcat(target,toStr);
  strcat(target, "]");
  if(move.isPromotion()){
    switch(move.getPromotionPiece()%6){
      case BISHOP:
        strcat(target,"b");
      break;
      case ROOK:
        strcat(target,"r");
      break;
      case KNIGHT:
        strcat(target,"n");
      break;
      case QUEEN:
        strcat(target,"q");
      break;
    }
  }
}
