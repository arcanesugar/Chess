#pragma once

#include "../Core/board.h"

Move search(Board b, int depth);//search uses minimax but returns a move instead of an evaluation
Move searchForMs(Board b, int ms);

//testing
void runMoveGenerationTest(Board *board);
void runMoveGenerationSuite();
