#include <iostream>
#include <string>
#include <random>

#include "Board/board.h"
#include "Search/search.h"
#include "ui/debug.h"
#include "ui/ui.h"

int main() {
  std::cout<<"[creating board...]\n";
  Board board;
  board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  board.enPassanTarget = 21;
  std::cout<<"[creating search...]\n";
  Search search;
  std::cout<<"[creating consoleInterface...]\n";
  ConsoleInterface consoleInterface;
  std::cout<<"[beginning consoleInterface...]\n";
  consoleInterface.run(board, search);
  return 0;
}