#pragma once

#include "../Board/board.h"

double minimax(Board *b, int depth, bool maximiser);
Move search(Board b, int depth);//search uses minimax but returns a move instead of an evaluation

//testing
void runMoveGenerationTest(Board *board);
void runMoveGenerationSuite();
