#pragma once

#include "../Core/board.h"

extern int pieceSquareTables[12][64];

double evaluate(Board *board);
void initEval();
