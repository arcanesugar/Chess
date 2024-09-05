#include <iostream>
#include <string>
#include <random>

#include "Board/board.h"
#include "debug.h"
#include "search.h"
#include "ui.h"

int main() {
  Board board;
  board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
  bool quit = false;
  Search search;
  search.init();
  debug::Settings printSettings;
  ConsoleState consoleState;
  while (!quit) {
    if(consoleState.printBoard) consoleState.output = "\n" + debug::printBoard(consoleState.settings,board) + consoleState.output;
    getNextInput(consoleState);
    std::string input = consoleState.lastInput;
    if (input == "mve") makeMoveFromConsole(board, consoleState);
    if(input == "dsp")  displaySettings(consoleState);
    if (input == "lgl") printLegalMoves(board, search, consoleState);
    if (input == "rnd") makeRandomMove(board, search, consoleState);
    if (input == "trn") whosTurnIsIt(board, consoleState);

    if(input == "hlp" || input == "help") showHelpMenu(consoleState);
  
  }
  return 0;
}