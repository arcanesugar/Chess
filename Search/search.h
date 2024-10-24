#pragma once
#include <float.h>
#include <chrono>
#include <cmath>
#include <random>
#include "../Board/board.h"
#include "../ui/debug.h"
#include "movegen.h"
#include "eval.h"

double minimax(Board &b, int depth, bool maximiser);
//testing
u64 perftTest(Board &b, int depth, bool root = true);

void runMoveGenerationTest(Board &board);
void runMoveGenerationSuite();
Move search(Board b, int depth);
