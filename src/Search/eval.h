#pragma once

#include "../Core/board.h"

extern int pieceSquareTables[12][64];

int evaluate(Board *board);
void initEval();
