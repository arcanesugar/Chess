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
      if(rstrEqual(&tl.tokens[1], "fen")){
        board = boardFromFEN(tl.tokens[2].buf);
      }
      if(rstrEqual(&tl.tokens[1], "startpos")){
        board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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
