#pragma once
#include <stack>
#include <iostream>

#include "../Board/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "debug.h"
#define INPUT_MAX_LEN 256
struct ConsoleState {
  printSettings settings;
  std::stack<Move> history;
  char lastInput[INPUT_MAX_LEN] = "";
  bool printBoard = true;
  Board *boardptr = nullptr;
};

extern ConsoleState consoleState;

void runConsoleInterface(Board *boardptr);//run the console interface
//
void getNextInput();
void showHelpMenu();
void displaySettings();
void whosTurnIsIt();
void makeMoveFromConsole();
void undoLastMove();
void makeRandomMove();
void printLegalMoves();
void showDebugView();
