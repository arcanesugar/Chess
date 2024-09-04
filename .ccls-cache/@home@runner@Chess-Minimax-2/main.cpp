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
    if (input == "rnd"){
      MoveList legalMoves;
      search.generateMoves(board, legalMoves);
      if(legalMoves.end <= 0) {
        consoleState.output = "No Legal Moves";
        consoleState.printBoard = true;
        continue;
      }
      Move move = legalMoves.moves[rand()%legalMoves.end];
      board.makeMove(move);
      consoleState.output = debug::printMove(printSettings, board, move);
      consoleState.output.append(std::to_string(legalMoves.end));
      consoleState.printBoard = false;
    }
    if (input == "trn") {
      if (board.flags & WHITE_TO_MOVE_BIT) {
        consoleState.output = "White to move";
      } else {
        consoleState.output = "Black to move";
      }
    }

    if(input == "hlp" || input == "help"){
      consoleState.output.append(
          (std::string)"---Help---\n"
        + "  trn - White/Black\n"
        + "  mve - Make Move\n"
        + "  lgl - Legal Moves\n"
        + "  dsp - Display Settings\n"
        + "  rnd - Random Move\n"
        + "  tst - TODO: add testing\n");
    }

  }
  return 0;
}