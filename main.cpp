#include <iostream>
#include <string>

#include "Board/board.h"
#include "Search/search.h"
#include "ui/ui.h"

int main() {
  std::cout<<"[creating board...]\n";
  Board board;
  board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  std::cout<<"[creating move generator]"<<std::endl;
  MoveGenerator moveGenerator;
  std::cout<<"[creating search...]\n";
  Search search;
  search.useMoveGenerator(&moveGenerator);
  std::cout<<"[creating consoleInterface...]\n";
  ConsoleInterface consoleInterface;
  std::cout<<"[beginning consoleInterface...]\n";
  consoleInterface.run(&board,&moveGenerator,&search);
  return 0;
}