#pragma once

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../Board/board.h"
#include "../ui/tostr.h"
#include "movegen.h"
#include "eval.h"

double minimax(Board *b, int depth, bool maximiser);
//testing
u64 perftTest(Board *b, int depth, bool root);

void runMoveGenerationTest(Board *board);
void runMoveGenerationSuite();
Move search(Board b, int depth);
