#include "CLI/ui.h"
#include "UCI/uci.h"
#include <stdio.h>

#include "io/io.h"

int main() {
  printf("Enter \"uci\" to enter uci, otherwise the engine will start in cli mode\n");
  rstr in = createRstr();
  rstrFromStdin(&in);
  if(rstrEqual(&in,"uci")){
    runUCI();
  }else{
    runConsoleInterface("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  }
  return 0;
}
