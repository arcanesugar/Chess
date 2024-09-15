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
  //board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  board.loadFromFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
  for(u64 bb : board.castlingMasks){
    std::cout<<debug::printBitboard(bb);
  }
  std::cout<<"[creating search...]\n";
  Search search;
  std::cout<<"[creating consoleInterface...]\n";
  ConsoleInterface consoleInterface;
  std::cout<<"[beginning consoleInterface...]\n";
  consoleInterface.run(board, search);
  return 0;
}