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
      runConsoleInterface("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      break;
  }

  return 0;
}
