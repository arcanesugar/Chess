#pragma once

#include "../Core/board.h"
#include "../Core/types.h"

struct printSettings{
  char pieceCharacters[14][6];
  //\x1b[<color>m
  int darkColor;
  int lightColor;
};
typedef struct printSettings printSettings;

void initPrintSettings(printSettings *settings);
void setUnicodePieces(printSettings *settings);
void setASCIIPieces(printSettings *settings);

void printPsqt(printSettings settings);
void printBoard(printSettings settings, Board board, u64 highlighted);
void printMoveOnBoard(printSettings settings, Board board, Move m);
void printBitboardOnBoard(printSettings settings,Board board,u64 bb);
