#include <string.h>
#include "CLI/cli.h"
#include "UCI/uci.h"
#include "Movegen/movegen.h"
#include "Search/eval.h"
#include "Search/zobrist.h"
#define KB 1024
#define MB 1024*KB

int main(int argc, char *argv[]) {
  initMoveGenerator();
  initEval();
  initTranspositionTable(32*MB);
  if(argc>1 && strcmp(argv[1],"cli") == 0)
    runConsoleInterface(STARTPOS_FEN);
  else
    runUCI();
  cleanupMoveGenerator();
  cleanupTranspositionTable();
  return 0;
}
