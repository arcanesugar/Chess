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


class Search{
  MoveGenerator moveGenerator;
  //testing
  u64 perftTest(Board &b, int depth, bool root = true);
  
public:
  Search();
  ~Search();
  void runMoveGenerationTest(Board &board);
  void runMoveGenerationSuite();
};
