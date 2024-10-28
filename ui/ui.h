#pragma once
#include <ctype.h>

#include "../Board/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "print.h"

#define INPUT_MAX_LEN 256


struct MoveStack{
  Move moves[255];
  int top;
};

struct ConsoleState {
  printSettings settings;
  MoveStack history;
  char lastInput[INPUT_MAX_LEN];
  bool printBoard;
  Board *boardptr;
};

extern ConsoleState consoleState;

void runConsoleInterface(Board *boardptr);

void getNextInput();
void showHelpMenu();
void displaySettings();
void whosTurnIsIt();
void makeMoveFromConsole();
void undoLastMove();
void makeRandomMove();
void printLegalMoves();
void makeBestMove(Board *boardptr);
void showDebugView();
