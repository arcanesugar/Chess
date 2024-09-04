#include <iostream>
#include <string>

#include "debug.h"
#include "board.h"

int main() {
  board Board;
  u64 bb = 0;
  setBit(bb,4);
  setBit(bb,6);
  setBit(bb,7);
  setBit(bb,10);
  Board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
  std::cout<<debug::testFormatting()<<std::endl;
  std::cout<<debug::printBitboard(bb);
  bb = bb>>3;
  std::cout<<debug::printBitboard(bb);
  std::cout<<"\n"<<debug::printIndices()<<"\n"<<debug::printBoard(Board); 
  std::cout<<debug::printBitboards(Board);
}