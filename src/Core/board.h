#pragma once

#include "types.h"
#include "move.h"

#define WHITE_TO_MOVE_BIT     0b00000001
#define WHITE_KINGSIDE_BIT    0b00000010
#define WHITE_QUEENSIDE_BIT   0b00000100
#define BLACK_KINGSIDE_BIT    0b00001000
#define BLACK_QUEENSIDE_BIT   0b00010000

#define WHITE_CASTLING_RIGHTS 0b00000110
#define BLACK_CASTLING_RIGHTS 0b00011000

#define EN_PASSAN_NULL 0

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

byte squareNameToIndex(char *squareName, int startIndex);
byte charToPiece(char c);
Board boardFromFEN(const char* fen);
char getSideToMove(Board *board);
char getOpponentColor(Board *board);

bool validateBoard(Board board);
void makeMove(Board *board, Move *m);
void unmakeMove(Board *board, Move *m);
#define STARTPOS_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
