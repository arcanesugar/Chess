#pragma once

#include "../Core/board.h"

Move iterativeDeepeningSearch(Board b, int maxDepth, int timeLimit, bool *quitWhenTrue);
//testing
void runMoveGenerationTest(Board *board);
void runMoveGenerationSuite();
