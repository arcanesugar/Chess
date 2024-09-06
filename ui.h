#pragma once
#include <iostream>

#include "Board/board.h"
#include "debug.h"
#include "search.h"

struct ConsoleState {
  debug::Settings settings;
  std::string lastInput = "";
  std::string output = "";
  bool printBoard = true;
};
class ConsoleInterface{
  ConsoleState c;

  byte squareNameToIndex(std::string squareName);

  void getNextInput();

  void showHelpMenu();
  void displaySettings();
  void whosTurnIsIt(Board &board);
  void makeMoveFromConsole(Board &board);
  void makeRandomMove(Board &board, Search &search);
  void printLegalMoves(Board &board, Search &search);

public:
  void run(Board &board, Search &search);//run the console interface
};