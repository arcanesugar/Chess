#include "uci.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "../Core/types.h"
#include "../io/rstr.h"
#include "../Core/board.h"
#include "../Search/search.h"
#include "../io/io.h"


enum SEARCH_STATES{
  IDLE,
  SEARCHING,
  DONE,
};
enum SEARCH_MODES{
  DEPTH,
  TIMED,
  INFINITE,
};

typedef struct UCIstate{
  Board board;
  int state;

  pthread_t searchThread;
  Move searchResult;//also readonly unless you are the search thread
  int searchState;//should be read only unless you are the search thread
  bool quitSearch;//if set to true the search thread should exit as soon as possible
  int searchDepth;
  int searchTime;
}UCIstate;

void* doSearch(void* args){
  UCIstate *state = (UCIstate*)args;
  state->searchState = SEARCHING;
  state->searchResult = iterativeDeepeningSearch(state->board,state->searchDepth,state->searchTime,&state->quitSearch);
  char moveStr[10] = "";
  moveToString(state->searchResult, moveStr);
  printf("bestmove %s\n",moveStr);
  state->searchState = DONE;
  return NULL;
};

void go(char* saveptr, UCIstate *state){
  if(state->searchState != IDLE) return;
  //default to an infinite search
  state->searchDepth = 256;
  state->searchTime  = 0;
  state->quitSearch = false;
  
  char *tokenptr;
  while(1){//never a bad idea
    tokenptr = strtok_r(NULL," ",&saveptr);
    if(tokenptr == NULL) break;
    if(strcmp(tokenptr,"depth") == 0){
      state->searchDepth = atoi(strtok_r(NULL," ",&saveptr));
      continue;
    }
    if(strcmp(tokenptr,"movetime") == 0){
      state->searchTime = atoi(strtok_r(NULL," ",&saveptr));
      if(state->searchTime >2) state->searchTime -=2;//make sure we quit before the search time is up
      continue;
    }
    if(strcmp(tokenptr,"infinite") == 0){
      state->searchDepth = 256;
      state->searchTime  = 0;
      continue;
    }
  }
  pthread_create(&state->searchThread,NULL,doSearch,state);
}

void position(char* saveptr, UCIstate *state){
  char *tokenptr = strtok_r(NULL, " ", &saveptr);
  if(strcmp(tokenptr,"fen") == 0){
    rstr fen = createRstr();
    for(int i = 0; i<6; i++){
      tokenptr = strtok_r(NULL, " ", &saveptr);
      if(!tokenptr || strcmp(tokenptr,"moves")) break;
      rstrAppend(&fen,tokenptr);
    }
    state->board = boardFromFEN(fen.buf);
  }
  else{//"startpos" assumed
    state->board = boardFromFEN(STARTPOS_FEN);
    strtok_r(NULL, " ", &saveptr);//consume "moves" token (if present)
  }
  while(1){
    tokenptr = strtok_r(NULL, " ", &saveptr);
    if(!tokenptr) break;
    Move move = moveFromStr(tokenptr,state->board);
    if(isNullMove(&move)) continue;
    makeMove(&state->board,&move);
  }
}

void uci(){
  //send name and author of engine
  printf("id name "ENGINE_NAME"\n");
  printf("id author "AUTHOR"\n");

  printf("uciok\n");
}

void runUCI(){
  rstr input = createRstr();
  bool quit = false;
  UCIstate state;
  state.searchState = IDLE;
  state.board = boardFromFEN(STARTPOS_FEN);
  setvbuf(stdout, NULL, _IONBF, 0);//turn off buffering for stdout(line buffering would also work, but just in case)
  while(!quit){
    rstrFromStream(&input,stdin);
    if(state.searchState == DONE){//I know that this wont happen until an input is provided, but it works for now
      pthread_join(state.searchThread ,NULL);
      state.searchState = IDLE;
    }
    char *saveptr;
    char *tokenptr;
    tokenptr = strtok_r(input.buf," ",&saveptr);
    if(strcmp(tokenptr, "position") == 0){position(saveptr, &state); continue;}
    if(strcmp(tokenptr, "quit") == 0){quit = true; continue;}
    if(strcmp(tokenptr, "isready") == 0){printf("readyok\n");}
    if(strcmp(tokenptr, "uci") == 0){uci(); continue;}
    if(strcmp(tokenptr, "go") == 0){go(saveptr, &state); continue;}
    if(strcmp(tokenptr, "stop") == 0){state.quitSearch = true; continue;}
    
    //debug commands
    if(strcmp(tokenptr, "d") == 0){
      printSettings ps;
      initPrintSettings(&ps);
      printBoard(ps, state.board, 0);
    }
  }
  if(state.searchState != IDLE){//just in case
    state.quitSearch = true;
    pthread_join(state.searchThread ,NULL);
  }
  destroyRstr(&input);
}
