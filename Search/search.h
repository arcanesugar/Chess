#pragma once
#include <chrono>
#include <fstream>
#include <map>//temporary 
#include <vector>
#include <cmath>
#include <random>
#include "../Board/board.h"
#include "../ui/debug.h"
#include "movegen.h"
#include "eval.h"

class Search{
  MoveGenerator *moveGenerator = nullptr;

  double nmax(Board &b, int depth);
  //testing
  u64 perftTest(Board &b, int depth, bool root = true);
public:
  void useMoveGenerator(MoveGenerator *mg) {moveGenerator = mg;};
  void runMoveGenerationTest(Board &board);
  void runMoveGenerationSuite();
  Move search(Board b, int depth);
};
