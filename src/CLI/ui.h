#pragma once

#include "../Core/board.h"

#define INPUT_MAX_LEN 256

struct MoveStack{
  Move moves[255];
  int top;
};
typedef struct MoveStack MoveStack;

void runConsoleInterface(const char* fen);
