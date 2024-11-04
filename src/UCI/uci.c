#include "uci.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Core/types.h"
#include "rstr.h"
#include "../Core/board.h"
#include "../Search/search.h"
#include "../ui/print.h"

#define MAX_TOKENS 50
struct TokenList{
  rstr tokens[MAX_TOKENS];
  int len;
};
typedef struct TokenList TokenList;

void createTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    tl->tokens[i].buf = NULL;
  }
}

void destroyTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    destroyRstr(&tl->tokens[i]);
  }
}

void tokeniseRstr(rstr *source, TokenList *tl){
  tl->len = 0;
  int tokenIndex = -1;
  bool creatingToken = false;
  bool inQuotes = false;
  for(int i = 0; i<rstrLen(source); i++){
    char c = rstrGetChar(source,i);
    if(c != ' ' && creatingToken == false){
      tokenIndex++;
      if(tl->tokens[tokenIndex].buf != NULL){
        rstrSet(&tl->tokens[tokenIndex],"");
      }else{
        tl->tokens[tokenIndex] = createRstr();
      }
      creatingToken = true;
    }
    if(creatingToken){
      if(c == ' ' && !inQuotes){
        creatingToken = false;
        continue;
      }
      if(c == '"'){
        inQuotes = !inQuotes;
        continue;
      }
      rstrAppendChar(&tl->tokens[tokenIndex],c);
    }
  }
  tl->len = tokenIndex+1;
}

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

void runUCI(){
  rstr input = createRstr();
  TokenList tl;
  createTokenList(&tl);
  bool quit = false;
  Board board;
  while(!quit){
    rstrFromStdin(&input);
    tokeniseRstr(&input,&tl);
    if(rstrEqual(&tl.tokens[0],"quit")){
      quit = true;
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"uci")){
      //send name and author of engine
      printf("id name "ENGINE_NAME"\n");
      printf("id author "AUTHOR"\n");

      printf("uciok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"isready")){
      printf("readyok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"position")){
      int movesStart = 0;
      if(rstrEqual(&tl.tokens[1], "fen")){
        board = boardFromFEN(tl.tokens[2].buf);
        if(rstrEqual(&tl.tokens[3], "moves")){
          movesStart = 4;
        }
      }
      if(rstrEqual(&tl.tokens[1], "startpos")){
        board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        if(rstrEqual(&tl.tokens[2], "moves")){
          movesStart = 3;
        }
      }
      if(movesStart !=0 && tl.len>movesStart){
        for(int tokenIndex = movesStart; tokenIndex<tl.len; tokenIndex++){
          Move move = moveFromStr(tl.tokens[tokenIndex].buf);
          makeMove(&board,&move);
        }
      }
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"d")){//stockfish also uses d to display the board
      printSettings ps = createDefaultPrintSettings();
      printBoard(ps, board, 0);
    }
    if(rstrEqual(&tl.tokens[0],"echo")){
      if(tl.len<2) {
        printf("not enough arguments");
        continue;
      }
      printf("%s\n", tl.tokens[1].buf);
    }
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}
