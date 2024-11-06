#include <string.h>
#include "CLI/cli.h"
#include "UCI/uci.h"

#include "settings.h"

int main(int argc, char *argv[]) {
  if(argc>1){//argc[0] is the name of the program, so theres always at least 1 argument
    if(strcmp(argv[1],"uci") == 0){
      runUCI();
      return 0;
    }
  }
  runConsoleInterface(STARTPOS_FEN);
  return 0;
}
