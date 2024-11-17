#include <string.h>
#include "CLI/cli.h"
#include "UCI/uci.h"
#include "Movegen/movegen.h"
#include "Search/eval.h"

int main(int argc, char *argv[]) {
  initMoveGenerator();
  initEval();
  if(argc>1 && strcmp(argv[1],"uci") == 0)
    runUCI();
  else
    runConsoleInterface(STARTPOS_FEN);
  cleanupMoveGenerator();
  return 0;
}
