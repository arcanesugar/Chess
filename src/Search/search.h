#pragma once

#include "../Core/board.h"

int nmax(Board *b, int depth, int alpha, int beta);
Move search(Board b, int depth);//search uses minimax but returns a move instead of an evaluation

//testing
void runMoveGenerationTest(Board *board);
void runMoveGenerationSuite();
