#pragma once
#include <iostream>

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
  ConsoleState c;
  Move last;
  byte squareNameToIndex(std::string squareName);

  void getNextInput();

  void showHelpMenu();
  void displaySettings();
  void whosTurnIsIt(Board &board);
  void makeMoveFromConsole(Board &board, Search &search);
  void makeRandomMove(Board &board, Search &search);
  void printLegalMoves(Board &board, Search &search);

public:
  void run(Board &board, Search &search);//run the console interface
};