#include "uci.h"
#include <stdio.h>

#include "../Core/types.h"
#include "../io/rstr.h"
#include "../Core/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "../Movegen/movegen.h"
#include "../io/print.h"
#include "../io/tokens.h"
#include "../io/io.h"
void go(TokenList *args){
  int nextArg = 1;
  int depth = -1;
  bool infinite = false;
  int movetime = -1;
  while(nextArg<args->len){
    if(rstrEqual(&args->tokens[nextArg], "depth")){
      depth = atoi(args->tokens[nextArg+1].buf);
      nextArg+=2;
      continue;
    }
    if(rstrEqual(&args->tokens[nextArg], "movetime")){
      movetime = atoi(args->tokens[nextArg+1].buf);
      nextArg+=2;
      continue;
    }
    if(rstrEqual(&args->tokens[nextArg], "infinite")){
      infinite = true;
      nextArg+=1;
      continue;
    }
    nextArg++;
  }
  if(depth != -1){
    printf("searching to depth %d\n", depth);
  }
  if(infinite){
    printf("searching until stop\n");
  }
  if(movetime != -1){
    printf("searching for %dms\n", movetime);
  }
}
void stop(){
  printf("stopping\n");
}

void runUCI(){
  rstr input = createRstr();
  TokenList tl;
  createTokenList(&tl);
  bool quit = false;
  Board board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  bool initialised = false;
  
  while(!quit){
    rstrFromStream(&input,stdin);
    tokeniseRstr(&input,&tl);
    if(tl.len == 0) continue;
    if(rstrEqual(&tl.tokens[0], "go")) {go(&tl); continue;}
    if(rstrEqual(&tl.tokens[0], "stop")) {stop(); continue;}
    if(rstrEqual(&tl.tokens[0], "quit")){
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
      if(!initialised){
        initialised = true;
        initEval();
        initMoveGenerator();   
      }
      printf("readyok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"position")){
      int movesStart = 0;
      if(rstrEqual(&tl.tokens[1], "fen")){
        board = boardFromFEN(tl.tokens[2].buf);
        if(tl.len == 3) continue;
        if(rstrEqual(&tl.tokens[3], "moves")){
          movesStart = 4;
        }
      }
      if(rstrEqual(&tl.tokens[1], "startpos")){
        board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        if(tl.len == 2) continue;
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
  }
  if(initialised){
    cleanupMoveGenerator();
    //eval doesnt need to be cleaned up
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}