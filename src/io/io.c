#include "io.h"
#include <stdio.h>
#include <string.h>

void rstrFromStdin(rstr *str){
  rstrSet(str,"");
  char buf[8];
  while(fgets(buf,8,stdin) != NULL){
    rstrAppend(str,buf);
    if(buf[strlen(buf)-1] == '\n') break;
  }
  rstrSetChar(str,rstrLen(str)-1,'\0');//remove newline character
}

static byte squareNameToIndex(char *squareName, int startIndex) {
  byte squareIndex =
      ((squareName[startIndex+1] - '0' - 1) * 8) + (7 - (squareName[startIndex] - 'a'));
  return squareIndex;
}

Move moveFromStr(char *str){
  Move move = createEmptyMove();
  if(strcmp(str, "ks") == 0){setSpecialMoveData(&move,CASTLE_KINGSIDE); return move;}
  if(strcmp(str, "qs") == 0){setSpecialMoveData(&move,CASTLE_QUEENSIDE); return move;}

  setFrom(&move,squareNameToIndex(str,0));
  setTo(&move,squareNameToIndex(str,2));
  if(strlen(str) == 5){
    byte piece;
    switch(str[4]){
      case 'p': piece = PAWN; break;
      case 'b': piece = BISHOP; break;
      case 'n': piece = KNIGHT; break;
      case 'r': piece = ROOK; break;
      case 'k': piece = KING; break;
      case 'q': piece = QUEEN; break;
    }
    setPromotion(&move,piece);
  };

  return move;
};
