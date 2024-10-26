#pragma once
#include <stack>

#include "../Board/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "print.h"

#define INPUT_MAX_LEN 256
struct ConsoleState {
  printSettings settings;
  std::stack<Move> history;
  char lastInput[INPUT_MAX_LEN] = "";
  bool printBoard = true;
  Board *boardptr = nullptr;
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
