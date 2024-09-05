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

byte squareNameToIndex(std::string squareName);

void getNextInput(ConsoleState &c);

void showHelpMenu(ConsoleState &c);
void displaySettings(ConsoleState &c);
void whosTurnIsIt(Board &board, ConsoleState &c);
void makeMoveFromConsole(Board &board, ConsoleState &c);
void makeRandomMove(Board &board, Search search,ConsoleState &c);
void printLegalMoves(Board &board, Search search, ConsoleState &c);