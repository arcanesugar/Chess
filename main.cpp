#include <iostream>
#include <string>

#include "Board/board.h"
#include "Search/search.h"
#include "ui/ui.h"

int main() {
  std::cout<<"[creating board...]\n";
  Board board;
  board.loadFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  std::cout<<"[creating search...]\n";
  Search search;
  std::cout<<"[creating consoleInterface...]\n";
  ConsoleInterface consoleInterface;
  std::cout<<"[beginning consoleInterface...]\n";
  consoleInterface.run(board, search);
  return 0;
}
