#ifdef CLI
#include "ui/ui.h"

int main() {
  runConsoleInterface("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  return 0;
}
#endif

#ifdef UCI
#include "UCI/uci.h"

int main() {
  runUCI();
  return 0;
}
#endif
