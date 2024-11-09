#pragma once
#include <stdio.h>
#include "../Core/move.h"
#include "rstr.h"

bool rstrFromStream(rstr *str, FILE* stream);//returns true if eof was reached. why? because I hate consistency
void moveToRstr(rstr *str, Move *m);
Move moveFromStr(char *str);//returns null if the string was not a valid move
