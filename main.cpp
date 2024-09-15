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
  board.loadFromFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
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