#include "io.h"
#include <stdio.h>
#include <string.h>

bool rstrFromStream(rstr *str, FILE* stream){
  rstrSet(str,"");
  char buf[8];//read 7 characters at a time
  bool eof = true;
  while(fgets(buf,8,stream) != NULL){
    rstrAppend(str,buf);
    if(buf[strlen(buf)-1] == '\n') {eof = false; break;}
  }
  if(rstrGetChar(str, rstrLen(str)-1) == '\n') rstrSetChar(str,rstrLen(str)-1,'\0');//remove newline character if present
  return eof;
}


Move moveFromStr(char *str, Board board){
  Move move = createEmptyMove();
  if(strcmp(str, "ks") == 0){setSpecialMoveData(&move,CASTLE_KINGSIDE); return move;}
  if(strcmp(str, "qs") == 0){setSpecialMoveData(&move,CASTLE_QUEENSIDE); return move;}

  if(str[0] < 'a' || str[0] > 'h') return createNullMove();
  if(str[2] < 'a' || str[2] > 'h') return createNullMove();

  if(str[1] < '1' || str[1] > '8') return createNullMove();
  if(str[3] < '1' || str[3] > '8') return createNullMove();

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
  struct MoveList legalMoves;
  generateMoves(&board, &legalMoves);
  bool isLegal = false;
  for(int i  =0; i<legalMoves.end; i++){
    if(getFrom(&move) == getFrom(&legalMoves.moves[i]) && getTo(&move) == getTo(&legalMoves.moves[i])){
      move = legalMoves.moves[i];
      isLegal = true;
      break;
    }
    if(isKingside(&legalMoves.moves[i])){
      if(strcmp(str,"e1g1") == 0 &&  (board.flags & WHITE_TO_MOVE_BIT)) return moveFromStr("ks", board);
      if(strcmp(str,"e1c1") == 0 && !(board.flags & WHITE_TO_MOVE_BIT)) return moveFromStr("ks", board);
    }
    if(isQueenside(&legalMoves.moves[i])){
      if(strcmp(str,"e8g8") == 0 &&  (board.flags & WHITE_TO_MOVE_BIT)) return moveFromStr("qs", board);
      if(strcmp(str,"e8c8") == 0 && !(board.flags & WHITE_TO_MOVE_BIT)) return moveFromStr("qs", board);
    }
  }
  if(!isLegal) return createNullMove();

  return move;
};
