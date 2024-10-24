#include "tostr.h"

void moveToString(Move move, char *target){
  if(isKingside(&move)){
    strcpy(target, "ks");
    return;
  }
  if(isQueenside(&move)){
    strcpy(target, "qs");
    return;
  }
  char fromStr[3] = "  ";
  char toStr[3] = "  ";
  fromStr[0] = 'h'-(getFrom(&move)%8);
  toStr[0] = 'h'-(getTo(&move)%8);
  fromStr[1] = '1'+(getFrom(&move)/8);
  toStr[1] = '1'+(getTo(&move)/8);
  
  strcpy(target, "[");
  strcat(target,fromStr);
  strcat(target, "->");
  strcat(target,toStr);
  strcat(target, "]");
  if(isPromotion(&move)){
    switch(getPromotionPiece(&move)%6){
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
