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

  int searchState;
  pthread_t searchThread;
}UCIstate;

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
  bool initialised = false;
  bool quit = false;
  UCIstate state;
  state.searchState = IDLE;
  state.board = boardFromFEN(STARTPOS_FEN);
  while(!quit){
    rstrFromStream(&input,stdin);
    tokeniseRstr(&input,&tl);
    if(tl.len == 0) continue;
    if(rstrEqual(&tl.tokens[0], "position")){position(&tl, &state); continue;}
    if(rstrEqual(&tl.tokens[0], "quit")){quit = true; continue;}
    if(rstrEqual(&tl.tokens[0],"isready")){isready(&initialised);}
    if(rstrEqual(&tl.tokens[0], "uci")){uci(); continue;}
    if(rstrEqual(&tl.tokens[0],"d")){//stockfish also uses d to display the board
      printSettings ps = createDefaultPrintSettings();
      printBoard(ps, state.board, 0);
    }
  }
  if(initialised){
    cleanupMoveGenerator();
    //eval doesnt need to be cleaned up
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}
