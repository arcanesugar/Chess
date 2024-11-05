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


enum SEARCH_STATE{
  IDLE,
  SEARCHING,
  DONE,
};

typedef struct UCIstate{
  Board board;
  int state;

  pthread_t searchThread;
  int searchState;//should be read only unless you are the search thread
  bool quitSearch;//if set to true the search thread should exit as soon as possible
  Move searchResult;//also readonly unless you are the search thread
}UCIstate;

void* doSearch(void* args){
  UCIstate *state = (UCIstate*)args;
  state->searchState = SEARCHING;
  state->searchResult = search(state->board,4);
  char moveStr[10] = "";
  moveToString(state->searchResult, moveStr);
  printf("bestmove %s\n",moveStr);
  state->searchState = DONE;
  return NULL;
};

void go(TokenList *args, UCIstate *state){
  if(state->searchState != IDLE) return;
  printf("Creating thread\n");
  pthread_create(&state->searchThread,NULL,doSearch,state);
}

void position(TokenList *args, UCIstate *state){
  int movesStart = 0;
  if(rstrEqual(&args->tokens[1], "fen")){
    state->board = boardFromFEN(args->tokens[2].buf);
    if(args->len == 3) return;
    if(rstrEqual(&args->tokens[3], "moves")){
      movesStart = 4;
    }
  }
  if(rstrEqual(&args->tokens[1], "startpos")){
    state->board = boardFromFEN(STARTPOS_FEN);
    if(args->len == 2) return;
    if(rstrEqual(&args->tokens[2], "moves")){
      movesStart = 3;
    }
  }
  if(movesStart !=0 && args->len>movesStart){
    for(int tokenIndex = movesStart; tokenIndex<args->len; tokenIndex++){
      Move move = moveFromStr(args->tokens[tokenIndex].buf);
      makeMove(&state->board,&move);
    }
  }
}

void isready(bool *initialised){
  if(!*initialised){
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
  bool initialised = false;
  bool quit = false;
  UCIstate state;
  state.searchState = IDLE;
  state.board = boardFromFEN(STARTPOS_FEN);
  while(!quit){
    rstrFromStream(&input,stdin);
    tokeniseRstr(&input,&tl);
    if(state.searchState == DONE){//I know that this wont happen until an input is provided, but it works for now
      pthread_join(state.searchThread ,NULL);
      state.searchState = IDLE;
    }
    if(tl.len == 0) continue;
    if(rstrEqual(&tl.tokens[0], "go")){go(&tl, &state); continue;}
    if(rstrEqual(&tl.tokens[0], "position")){position(&tl, &state); continue;}
    if(rstrEqual(&tl.tokens[0], "quit")){quit = true; continue;}
    if(rstrEqual(&tl.tokens[0], "isready")){isready(&initialised);}
    if(rstrEqual(&tl.tokens[0], "uci")){uci(); continue;}
    
    //debug commands
    if(rstrEqual(&tl.tokens[0], "d")){
      printSettings ps = createDefaultPrintSettings();
      printBoard(ps, state.board, 0);
    }
  }
  if(state.searchState == DONE)
    pthread_join(state.searchThread ,NULL);
  if(initialised){
    cleanupMoveGenerator();
    //eval doesnt need to be cleaned up
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}
