#pragma once
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "Bitboards/bitboard.h"
#include "../types.h"

#define EN_PASSAN_NULL    0

#define WHITE_TO_MOVE_BIT    0b00000001
#define WHITE_KINGSIDE_BIT   0b00000010
#define WHITE_QUEENSIDE_BIT  0b00000100
#define BLACK_KINGSIDE_BIT   0b00001000
#define BLACK_QUEENSIDE_BIT  0b00010000

#define WHITE_CASTLING_RIGHTS 0b00000110
#define BLACK_CASTLING_RIGHTS 0b00011000

enum piece{
  PAWN = 0,
  BISHOP,
  KNIGHT,
  ROOK,
  QUEEN,
  KING
};

#define WHITE 0
#define BLACK 6
#define EMPTY 12

#define WHITE_PIECES 12
#define BLACK_PIECES 13

int getSquareIndex(int file, int rank);

extern u64 rankMasks[8];
extern u64 fileMasks[8];
void generateBoardMasks();

struct Board{
  u64 bitboards[14];
  u64 occupancy; 
  byte enPassanTarget;
  byte squares[64];
  byte flags;
};
typedef struct Board Board;
Board boardFromFEN(char* fen);
bool validateBoard(Board board);

void makeMove(Board *board, Move *m);
void unmakeMove(Board *board, Move *m);
