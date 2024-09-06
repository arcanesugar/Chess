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
  Search search;
  search.init();
  ConsoleInterface consoleInterface;
  consoleInterface.run(board, search);
  return 0;
}