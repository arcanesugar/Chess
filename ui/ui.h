#pragma once
#include <iostream>
#include <stack>
#include <string>

#include "../Board/board.h"
#include "../Search/search.h"
#include "../Search/eval.h"
#include "debug.h"

struct ConsoleState {
  debug::Settings settings;
  std::stack<Move> history;
  std::string lastInput = "";
  std::string output = "";
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
