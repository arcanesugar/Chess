#pragma once
#include "../Board/board.h"

struct printSettings{
  char pieceCharacters[14][6];
  char darkColor[10]  = "\x1b[103m";
  char lightColor[10] = "\x1b[47m";
};

void setUnicodePieces(printSettings *settings);
void setASCIIPieces(printSettings *settings);
void printBoard(printSettings settings, Board board, u64 highlighted = u64(0));

void printMoveOnBoard(printSettings settings, Board board, Move m);
void printBitboardOnBoard(printSettings settings,Board board,u64 bb);
