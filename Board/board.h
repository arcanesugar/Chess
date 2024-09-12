#pragma once
#include <iostream>
#include <string>
#include <algorithm>

#include "Bitboards/bitboard.h"

#define CAPTURE_BIT       0b00000001
#define KINGSIDE_BIT      0b00000010
#define QUEENSIDE_BIT     0b00000100
#define EN_PASSAN 16

#define WHITE_TO_MOVE_BIT 0b00000001

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
struct Move{
  byte from = 0;
  byte to = 0;
  byte flags = 0; //last(leftmost) 4 bits are the ID of the captured piece, if there was a capture
};

int getSquareIndex(int file, int rank);

struct Board{
  u64 bitboards[14];
  u64 occupancy;//1 if a piece is present
  byte squares[64];
  byte flags = 0 | WHITE_TO_MOVE_BIT;
  
  void makeMove(Move &m);
  void unmakeMove(Move &m);
  void loadFromFEN(std::string fen);
  void updateColorBitboards();
};