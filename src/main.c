#include <stdio.h>
#include "Core/board.h"
#include "ui/ui.h"
#include "Movegen/movegen.h"
#include "Movegen/Magic/magic.h"

int main() {
  printf("[creating board...]\n");
  char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Board board = boardFromFEN(fen);
  generateBoardMasks();
  printf("[creating move generator]\n");
  initMoveGenerator();
  printf("[beginning consoleInterface...]\n");
  runConsoleInterface(&board);
  cleanupMagics();
  return 0;
}
