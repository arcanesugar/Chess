#include "uci.h"
#include <stdio.h>
#include <pthread.h>

#include "../Core/types.h"
#include "../io/rstr.h"
#include "../Core/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "../Movegen/movegen.h"
#include "../io/print.h"
#include "../io/tokens.h"
#include "../io/io.h"

//I should stop using so many globals
typedef struct SearchState{
  bool isSearching;
  pthread_t thread;
  Move result;
  
  Board position;
  int depth;
  bool infinite;
  int movetime;
}SearchState;
static SearchState searchState;

void go(TokenList *args, Board *board){
  if(searchState.isSearching) return;
  searchState.position = *board;
  searchState.depth    = -1;
  searchState.movetime = -1;
  searchState.infinite = false;
  int nextArg = 1;
  while(nextArg<args->len){
    if(rstrEqual(&args->tokens[nextArg], "depth")){
      searchState.depth = atoi(args->tokens[nextArg+1].buf);
      nextArg+=2;
      continue;
    }
    if(rstrEqual(&args->tokens[nextArg], "movetime")){
      searchState.movetime = atoi(args->tokens[nextArg+1].buf);
      nextArg+=2;
      continue;
    }
    if(rstrEqual(&args->tokens[nextArg], "infinite")){
      searchState.infinite = true;
      nextArg+=1;
      continue;
    }
    nextArg++;
  }
}

void position(TokenList *args, Board *board){
  int movesStart = 0;
  if(rstrEqual(&args->tokens[1], "fen")){
    *board = boardFromFEN(args->tokens[2].buf);
    if(args->len == 3) return;
    if(rstrEqual(&args->tokens[3], "moves")){
      movesStart = 4;
    }
  }
  if(rstrEqual(&args->tokens[1], "startpos")){
    *board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    if(args->len == 2) return;
    if(rstrEqual(&args->tokens[2], "moves")){
      movesStart = 3;
    }
  }
  if(movesStart !=0 && args->len>movesStart){
    for(int tokenIndex = movesStart; tokenIndex<args->len; tokenIndex++){
      Move move = moveFromStr(args->tokens[tokenIndex].buf);
      makeMove(board,&move);
    }
  }
}

void isready(bool *initialised){
  if(!initialised){
    *initialised = true;
    initEval();
    initMoveGenerator();   
  }
  printf("readyok\n");
  return;
}
void uci(){
  //send name and author of engine
  printf("id name "ENGINE_NAME"\n");
  printf("id author "AUTHOR"\n");

  printf("uciok\n");
}

void runUCI(){
  TokenList tl;
  createTokenList(&tl);
  rstr input = createRstr();
  Board board;
  bool initialised = false;
  bool quit = false;
  while(!quit){
    rstrFromStream(&input,stdin);
    tokeniseRstr(&input,&tl);
    if(tl.len == 0) continue;
    if(rstrEqual(&tl.tokens[0], "go")){go(&tl, &board); continue;}
    if(rstrEqual(&tl.tokens[0], "position")){position(&tl, &board); continue;}
    if(rstrEqual(&tl.tokens[0], "quit")){quit = true; continue;}
    if(rstrEqual(&tl.tokens[0],"isready")){isready(&initialised);}
    if(rstrEqual(&tl.tokens[0], "uci")){uci(); continue;}
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