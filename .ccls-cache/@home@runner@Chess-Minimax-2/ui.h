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

void makeMoveFromConsole(Board board, ConsoleState &c);
void getNextInput(ConsoleState &c);
byte squareNameToIndex(std::string squareName);
void displaySettings(ConsoleState &c);
void printLegalMoves(Board board, Search search, ConsoleState &c);