#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../Movegen/Magic/magic.h"
#include "../Movegen/movegen.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "print.h"
#include "ui.h"

typedef struct ConsoleState ConsoleState;
struct ConsoleState {
  printSettings settings;
  MoveStack history;
  char lastInput[INPUT_MAX_LEN];
  bool printBoard;
  Board *boardptr;
};
static ConsoleState consoleState;

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
void displaySettings();
void whosTurnIsIt();
void makeMoveFromConsole();
void undoLastMove();
void makeRandomMove();
void printLegalMoves();
void makeBestMove(Board *boardptr);
void showDebugView();

void getNextInput() {
  printf(">>");
  char temp[INPUT_MAX_LEN];
  if(fgets(temp,INPUT_MAX_LEN,stdin) == NULL) return;
  if(strlen(temp) > 1) {
    temp[strlen(temp)-1] = '\0';//remove newline character
    strcpy(consoleState.lastInput,temp);
  }
  consoleState.printBoard = true;
}

void runConsoleInterface(Board *boardptr){
  setUnicodePieces(&consoleState.settings);
  setLightColor(&consoleState.settings, "47");
  setDarkColor(&consoleState.settings, "103");
  consoleState.printBoard = true;
  consoleState.boardptr = boardptr;
  consoleState.history = createMoveStack();
  consoleState.lastInput[0] = '\0';
  bool quit = false;
  while (!quit) {
    if(consoleState.printBoard) printBoard(consoleState.settings,*boardptr,0);
    getNextInput();
    if(strcmp(consoleState.lastInput, "mve") == 0) {makeMoveFromConsole(); continue;}
    if(strcmp(consoleState.lastInput, "evl") == 0) {printf("%f\n",evaluate(boardptr)); continue;}
    if(strcmp(consoleState.lastInput, "bst") == 0) {makeBestMove(boardptr); continue;}
    if(strcmp(consoleState.lastInput, "dsp") == 0) {displaySettings(); continue;}
    if(strcmp(consoleState.lastInput, "lgl") == 0) {printLegalMoves(); continue;}
    if(strcmp(consoleState.lastInput, "rnd") == 0) {makeRandomMove(); continue;}
    if(strcmp(consoleState.lastInput, "trn") == 0) {whosTurnIsIt(); continue;}
    if(strcmp(consoleState.lastInput, "hlp") == 0) {showHelpMenu(); continue;}
    if(strcmp(consoleState.lastInput, "sch") == 0) {searchForMagics(); continue;}
    if(strcmp(consoleState.lastInput, "tst") == 0) {runMoveGenerationTest(boardptr); continue;}
    if(strcmp(consoleState.lastInput, "mgs") == 0) {runMoveGenerationSuite(); continue;}
    if(strcmp(consoleState.lastInput, "und") == 0) {undoLastMove();  continue;}
    if(strcmp(consoleState.lastInput, "dbg") == 0) {showDebugView(); continue;}
    if(strcmp(consoleState.lastInput, "psq") == 0) {printPsqt(consoleState.settings); continue;}
    if(strcmp(consoleState.lastInput, "q") == 0) quit = true;


    if(strcmp(consoleState.lastInput, "help") == 0) {showHelpMenu(); continue;}
  }
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
  printf("  q   - Quit\n");

  printf("\n---Debug commands---\n");
  printf("  dbg - Debug View\n");
  printf("  psq - Show piece square tables\n");
  printf("  evl - Show positions evaluation\n");
  printf("  sch - Search for magic numbers\n");
  printf("  tst - Run move generation test on current position\n");
  printf("  mgs - Run move generation test suite\n");
}

void makeBestMove(Board *boardptr){
  Move best = search(*boardptr, 4);
  if(isNullMove(&best)){
    printf("No legal moves(Checkmate)\n");
    return;
  }
  makeMove(boardptr,&best);
  moveStackPush(&consoleState.history,best);
  consoleState.printBoard = false;
  printMoveOnBoard(consoleState.settings, *boardptr, best);
}

void undoLastMove(){
  if(moveStackEmpty(consoleState.history)){
    printf("No more move history is avalible\n\x1b[2m(If you believe this is a mistake, contact your local library)\n\x1b[0m\n");
    return;
  }
  Move m = moveStackPeek(&consoleState.history);
  unmakeMove(consoleState.boardptr,&m);
  printMoveOnBoard(consoleState.settings,*consoleState.boardptr, m);
  consoleState.printBoard = false;

  consoleState.history.top--;
}

void whosTurnIsIt(){
  if (consoleState.boardptr->flags & WHITE_TO_MOVE_BIT) {
    printf("White to move"); return;
  }
  printf("Black to move"); return;
}

void makeRandomMove(){
  struct MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  if(legalMoves.end <= 0) {
    printf("No legal moves\n");
    consoleState.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  makeMove(consoleState.boardptr,&move);
  moveStackPush(&consoleState.history,move);
  printMoveOnBoard(consoleState.settings, *consoleState.boardptr, move);
  consoleState.printBoard = false;
}

void printLegalMoves(){
  struct MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  printf("%i moves printed\n", legalMoves.end);
  for (int i = 0; i < legalMoves.end; i++) {
    printMoveOnBoard(consoleState.settings, *consoleState.boardptr, legalMoves.moves[i]);
  }
  consoleState.printBoard = false;
}

byte squareNameToIndex(char *squareName, int startIndex) {
  byte squareIndex =
      ((squareName[startIndex+1] - '0' - 1) * 8) + (7 - (squareName[startIndex] - 'a'));
  return squareIndex;
}

void makeMoveFromConsole(){
  printf("move(eg e2e4, a7a8n)\n");
  printf("castle with \"ks\" and \"qs\"\n");
  getNextInput();
  Move move;
  if(strcmp(consoleState.lastInput, "ks") == 0){setSpecialMoveData(&move,CASTLE_KINGSIDE);}
  if(strcmp(consoleState.lastInput, "qs") == 0){setSpecialMoveData(&move,CASTLE_QUEENSIDE);}
  if(consoleState.lastInput[0] != 'k' && consoleState.lastInput[0] != 'q'){
    setFrom(&move,squareNameToIndex(consoleState.lastInput,0));
    setTo(&move,squareNameToIndex(consoleState.lastInput,2));
    if(strlen(consoleState.lastInput) == 5){
      byte piece;
      switch(consoleState.lastInput[4]){
        case 'p': piece = PAWN; break;
        case 'b': piece = BISHOP; break;
        case 'n': piece = KNIGHT; break;
        case 'r': piece = ROOK; break;
        case 'k': piece = KING; break;
        case 'q': piece = QUEEN; break;
        default:
          printf("Invalid promotion");
          return;
          break;
      }
      setPromotion(&move,piece);
    };
  }
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  bool isLegal = false;
  for(int i  =0; i<legalMoves.end; i++){
    if(getFrom(&move) == getFrom(&legalMoves.moves[i]) && getTo(&move) == getTo(&legalMoves.moves[i])){
      isLegal = true;
      break;
    }
  }
  if(isLegal){
    makeMove(consoleState.boardptr,&move);
    moveStackPush(&consoleState.history,move);
  }else{
    printf("This move is not legal, continue? (y/N)\n");
    getNextInput();
    if(consoleState.lastInput[0] == 'y'){
      makeMove(consoleState.boardptr,&move);
      moveStackPush(&consoleState.history,move);
    }
  }
  printMoveOnBoard(consoleState.settings, *consoleState.boardptr, move);
  consoleState.printBoard = false;
}

void displaySettings(){
  bool done = false;
  while(!done){
    printf("---Display Settings---\n  ");
    for(int i = 0; i<12; i++)
      printf("%s ", consoleState.settings.pieceCharacters[i]);
    printf("\n  Dark %s  \x1b[0m Light %s  \x1b[0m\n", consoleState.settings.darkColor, consoleState.settings.lightColor);
    printf("  0 - Use Unicode Pieces\n");
    printf("  1 - Use ASCII Pieces\n");
    printf("  q - Done\n");
    getNextInput();
    if(consoleState.lastInput[0] == 'q') return;
    if(!isdigit(consoleState.lastInput[0])) continue;
    switch(atoi(consoleState.lastInput)){
      case 0:
        setUnicodePieces(&consoleState.settings);
      break;
      case 1:
        setASCIIPieces(&consoleState.settings);
      break;
    }
  }
}

void showDebugView(){
  consoleState.printBoard = false;
  for(int i = 0; i<14; i++){
    printBitboardOnBoard(consoleState.settings, *consoleState.boardptr, consoleState.boardptr->bitboards[i]);
    printf("\n");
  }
  for(int i = 7; i>=0; i--){
    if(consoleState.boardptr->flags>>i&1){
      printf("1");
    }else{
      printf("0");
    }
  }
  printf("\n");
}
