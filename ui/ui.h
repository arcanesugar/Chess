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
  std::string lastInput = "";
  std::string output = "";
  bool printBoard = true;
};

class ConsoleInterface{
  Board *boardptr = nullptr;
  ConsoleState c;
  std::stack<Move> history;
  byte squareNameToIndex(std::string squareName);

  void getNextInput();

  void showHelpMenu();
  
  void displaySettings();
  void whosTurnIsIt();
  void makeMoveFromConsole();
  void undoLastMove();
  void makeRandomMove();
  void printLegalMoves();
  void showDebugView();
public:
  void run(Board *boardptr);//run the console interface
};
