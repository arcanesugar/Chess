#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../Movegen/Magic/magic.h"
#include "../Movegen/movegen.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "../io/print.h"
#include "../io/io.h"
#include "cli.h"

typedef struct ConsoleState ConsoleState;
struct ConsoleState {
  printSettings settings;
  MoveStack history;
  bool printBoard;
  Board board;
  rstr lastInput;
};

MoveStack createMoveStack(){
  MoveStack ms;
  ms.top = -1;
  return ms;
}
void moveStackPush(MoveStack *ms, Move m){
  ms->moves[++ms->top] = m;
}
Move moveStackPeek(MoveStack *ms){
  return ms->moves[ms->top];
}
bool moveStackPop(MoveStack *ms){
  if(ms->top == 1) return false;
  ms->top--;
  return true;
}
bool moveStackEmpty(MoveStack ms){
  if(ms.top == -1) return true;
  return false;
}

//function prototypes so runConsoleInterface can be at the top of the file
void showHelpMenu();
void displaySettings(ConsoleState *state);
void whosTurnIsIt(ConsoleState *state);
void makeMoveFromConsole(ConsoleState *state);
void undoLastMove(ConsoleState *state);
void makeRandomMove(ConsoleState *state);
void printLegalMoves(ConsoleState *state);
void makeBestMove(ConsoleState *state);
void showDebugView(ConsoleState *state);
void playAgainstSelf(ConsoleState *state);

void getNextInput(ConsoleState *state) {
  printf(">>");
  rstr prev = createRstr();
  rstrSet(&prev,state->lastInput.buf);
  rstrFromStream(&state->lastInput, stdin);
  if(rstrLen(&state->lastInput) == 0){
    rstrSet(&state->lastInput, prev.buf);
    printf("no command given, repeating last (%s)\n", prev.buf);
  } 
  destroyRstr(&prev);
  state->printBoard = true;
}
void initEngine(){
  printf("[creating move generator]\n");
  initMoveGenerator();
  initEval();
}
void cleanupEngine(){
  cleanupMagics();
}

void runConsoleInterface(const char* fen){
  ConsoleState state;
  printf("[creating board]\n");
  setUnicodePieces(&state.settings);
  setLightColor(&state.settings, "47");
  setDarkColor(&state.settings, "103");
  state.printBoard = true;
  state.board = boardFromFEN(fen);
  state.history = createMoveStack();
  state.lastInput = createRstr();
  bool quit = false;
  initEngine();
  while (!quit) {
    if(state.printBoard) printBoard(state.settings,state.board,0);
    getNextInput(&state);
    if(rstrEquals(&state.lastInput, "mve")) {makeMoveFromConsole(&state); continue;}
    if(rstrEquals(&state.lastInput, "evl")) {printf("%f\n",evaluate(&state.board)); continue;}
    if(rstrEquals(&state.lastInput, "bst")) {makeBestMove(&state); continue;}
    if(rstrEquals(&state.lastInput, "dsp")) {displaySettings(&state); continue;}
    if(rstrEquals(&state.lastInput, "lgl")) {printLegalMoves(&state); continue;}
    if(rstrEquals(&state.lastInput, "rnd")) {makeRandomMove(&state); continue;}
    if(rstrEquals(&state.lastInput, "trn")) {whosTurnIsIt(&state); continue;}
    if(rstrEquals(&state.lastInput, "hlp")) {showHelpMenu(); continue;}
    if(rstrEquals(&state.lastInput, "sch")) {searchForMagics(); continue;}
    if(rstrEquals(&state.lastInput, "tst")) {runMoveGenerationTest(&state.board); continue;}
    if(rstrEquals(&state.lastInput, "mgs")) {runMoveGenerationSuite(); continue;}
    if(rstrEquals(&state.lastInput, "und")) {undoLastMove(&state);  continue;}
    if(rstrEquals(&state.lastInput, "dbg")) {showDebugView(&state); continue;}
    if(rstrEquals(&state.lastInput, "psq")) {printPsqt(state.settings); continue;}
    if(rstrEquals(&state.lastInput, "ply")) {playAgainstSelf(&state); continue;}
    if(rstrEquals(&state.lastInput, "q")) quit = true;


    if(rstrEquals(&state.lastInput, "help")) {showHelpMenu(); continue;}
    if(rstrEquals(&state.lastInput, "quit")) quit = true;
  }
  destroyRstr(&state.lastInput);
  cleanupEngine();
}

void showHelpMenu(){
  printf("---Help---\n");
  printf("  mve - Make move\n");
  printf("  rnd - Random move\n");
  printf("  und - Undo last move\n");
  printf("  trn - Who's turn is it\n");
  printf("  lgl - List legal moves\n");
  printf("  dsp - Display settings\n");
  printf("  hlp - Show this list\n");
  printf("  ply - Play against self\n");
  printf("  q  - Quit\n\n");
  
  printf("  help - Show this list\n");
  printf("  quit - Quit\n");
  printf("\n---Debug commands---\n");
  printf("  dbg - Debug View\n");
  printf("  psq - Show piece square tables\n");
  printf("  evl - Show positions evaluation\n");
  printf("  sch - Search for magic numbers\n");
  printf("  tst - Run move generation test on current position\n");
  printf("  mgs - Run move generation test suite\n");
}

void playAgainstSelf(ConsoleState *state){
  bool checkmate = false;
  while(!checkmate){
    Move best = iterativeDeepeningSearch(state->board, 256, 2000, NULL);
    if(isNullMove(&best)){
      checkmate = true;
    }
    makeMove(&state->board,&best);
    moveStackPush(&state->history,best);
    state->printBoard = false;
    printMoveOnBoard(state->settings, state->board, best);
  }
  printf("checkmate");
}

void makeBestMove(ConsoleState *state){
  Move best = iterativeDeepeningSearch(state->board, 256, 2000, NULL);
  if(isNullMove(&best)){
    printf("No legal moves(Checkmate)\n");
    return;
  }
  makeMove(&state->board,&best);
  moveStackPush(&state->history,best);
  state->printBoard = false;
  printMoveOnBoard(state->settings, state->board, best);
}

void undoLastMove(ConsoleState *state){
  if(moveStackEmpty(state->history)){
    printf("No more move history is avalible\n\x1b[2m(If you believe this is a mistake, contact your local library)\n\x1b[0m\n");
    return;
  }
  Move m = moveStackPeek(&state->history);
  unmakeMove(&state->board,&m);
  printMoveOnBoard(state->settings,state->board, m);
  state->printBoard = false;

  state->history.top--;
}

void whosTurnIsIt(ConsoleState *state){
  if (state->board.flags & WHITE_TO_MOVE_BIT) {
    printf("White to move"); return;
  }
  printf("Black to move"); return;
}

void makeRandomMove(ConsoleState *state){
  struct MoveList legalMoves;
  generateMoves(&state->board, &legalMoves);
  if(legalMoves.end <= 0) {
    printf("No legal moves\n");
    state->printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  makeMove(&state->board,&move);
  moveStackPush(&state->history,move);
  printMoveOnBoard(state->settings, state->board, move);
  state->printBoard = false;
}

void printLegalMoves(ConsoleState *state){
  struct MoveList legalMoves;
  generateMoves(&state->board, &legalMoves);
  printf("%i moves printed\n", legalMoves.end);
  for (int i = 0; i < legalMoves.end; i++) {
    printMoveOnBoard(state->settings, state->board, legalMoves.moves[i]);
  }
  state->printBoard = false;
}

void makeMoveFromConsole(ConsoleState *state){
  printf("move(eg e2e4, a7a8n)\n");
  printf("castle with \"ks\" and \"qs\"\n");
  getNextInput(state);
  Move move = moveFromStr(state->lastInput.buf);
  MoveList legalMoves;
  generateMoves(&state->board, &legalMoves);
  bool isLegal = false;
  for(int i  =0; i<legalMoves.end; i++){
    if(getFrom(&move) == getFrom(&legalMoves.moves[i]) && getTo(&move) == getTo(&legalMoves.moves[i])){
      move = legalMoves.moves[i];
      isLegal = true;
      break;
    }
  }
  if(!isLegal){
    printf("This move is not legal, continue? (y/N)\n");
    getNextInput(state);
    if(state->lastInput.buf[0] != 'y') return;
  }
  makeMove(&state->board,&move);
  moveStackPush(&state->history,move);
  printMoveOnBoard(state->settings, state->board, move);
  state->printBoard = false;
}

void displaySettings(ConsoleState *state){
  bool done = false;
  while(!done){
    printf("---Display Settings---\n  ");
    for(int i = 0; i<12; i++)
      printf("%s ", state->settings.pieceCharacters[i]);
    printf("\n  Dark %s  \x1b[0m Light %s  \x1b[0m\n", state->settings.darkColor, state->settings.lightColor);
    printf("  0 - Use Unicode Pieces\n");
    printf("  1 - Use ASCII Pieces\n");
    printf("  q - Done\n");
    getNextInput(state);
    if(state->lastInput.buf[0] == 'q') return;
    if(!isdigit(state->lastInput.buf[0])) continue;
    switch(atoi(state->lastInput.buf)){
      case 0:
        setUnicodePieces(&state->settings);
      break;
      case 1:
        setASCIIPieces(&state->settings);
      break;
    }
  }
}

void showDebugView(ConsoleState *state){
  state->printBoard = false;
  for(int i = 0; i<14; i++){
    printBitboardOnBoard(state->settings, state->board, state->board.bitboards[i]);
    printf("\n");
  }
  for(int i = 7; i>=0; i--){
    if(state->board.flags>>i&1){
      printf("1");
    }else{
      printf("0");
    }
  }
  printf("\n");
}
