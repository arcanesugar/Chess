#include "CLI/ui.h"
#include "UCI/uci.h"

#include "settings.h"

int main() {
  loadSettings();
  switch(settings.mode){
    case MODE_UCI:
      runUCI();
      break;
    case MODE_CLI:
      runConsoleInterface(STARTPOS_FEN);
      break;
  }

  return 0;
}
