#pragma once
#include <stdio.h>
#include "../Core/move.h"
#include "rstr.h"
#include "print.h"
#include "../Core/board.h"
#include "../Movegen/movegen.h"

bool rstrFromStream(rstr *str, FILE* stream);//returns true if eof was reached. why? because I hate consistency
Move moveFromStr(char *str, Board board);
