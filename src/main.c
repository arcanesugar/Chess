#include <string.h>
#include "CLI/cli.h"
#include "UCI/uci.h"
#include "Movegen/movegen.h"
#include "Search/eval.h"
#include "Search/zobrist.h"

int main(int argc, char *argv[]) {
  initMoveGenerator();
  initEval();
  initTranspositionTable(100);
  if(argc>1 && strcmp(argv[1],"uci") == 0)
    runUCI();
  else
    runConsoleInterface(STARTPOS_FEN);
  cleanupMoveGenerator();
  cleanupTranspositionTable();
  return 0;
}
