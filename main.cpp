#include <stdio.h>

#include "Board/board.h"
#include "Search/search.h"
#include "ui/ui.h"

int main() {
  printf("[creating board...]\n");
  char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  char fen2[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
  Board board = boardFromFEN(fen);
  board = boardFromFEN(fen2);
  generateBoardMasks();
  printf("[creating move generator]\n");
  initMoveGenerator();
  printf("[creating consoleInterface...]\n");
  ConsoleInterface consoleInterface;
  printf("[beginning consoleInterface...]\n");
  consoleInterface.run(&board);
  return 0;
}
