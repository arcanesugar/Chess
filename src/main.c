#include <string.h>
#include "CLI/cli.h"
#include "UCI/uci.h"
#include "Movegen/movegen.h"
#include "Search/eval.h"

int main(int argc, char *argv[]) {
  initMoveGenerator();
  initEval();
  if(argc>1){//argc[0] is the name of the program, so theres always at least 1 argument
    if(strcmp(argv[1],"uci") == 0){
      runUCI();
      cleanupMoveGenerator();
      return 0;
    }
  }
  runConsoleInterface(STARTPOS_FEN);
  cleanupMoveGenerator();
  return 0;
}
