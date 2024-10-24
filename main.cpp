#include <stdio.h>

#include "Board/board.h"
#include "Search/search.h"
#include "ui/ui.h"

int main() {
  printf("[creating board...]\n");
  Board board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");;
  generateBoardMasks();
  printf("[creating move generator]\n");
  initMoveGenerator();
  printf("[creating consoleInterface...]\n");
  ConsoleInterface consoleInterface;
  printf("[beginning consoleInterface...]\n");
  consoleInterface.run(&board);
  return 0;
}
