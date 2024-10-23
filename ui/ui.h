#pragma once
#include <iostream>
#include <stack>

#include "../Board/board.h"
#include "../Search/search.h"
#include "debug.h"

struct ConsoleState {
  debug::Settings settings;
  std::string lastInput = "";
  std::string output = "";
  bool printBoard = true;
};
class ConsoleInterface{
  Search *searchptr = nullptr;
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
  void run(Board *boardptr,Search *searchptr);//run the console interface
};
