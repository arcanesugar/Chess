#pragma once

#include "../Core/board.h"
#include "../Core/types.h"

struct printSettings{
  char pieceCharacters[14][6];
  char darkColor[10];
  char lightColor[10];
};

typedef struct printSettings printSettings;

//set light and dark color will result in \x1b[colorIDm
void setLightColor(printSettings *settings, const char *colorID);
void setDarkColor(printSettings *settings, const char *colorID);
void setUnicodePieces(printSettings *settings);
void setASCIIPieces(printSettings *settings);

void printPsqt(printSettings settings);
void printBoard(printSettings settings, Board board, u64 highlighted);
void printMoveOnBoard(printSettings settings, Board board, Move m);
void printBitboardOnBoard(printSettings settings,Board board,u64 bb);
