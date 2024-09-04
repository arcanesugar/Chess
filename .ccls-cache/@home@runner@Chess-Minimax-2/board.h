#pragma once
#include <iostream>
#include <string>
#include <algorithm>

#include "bitboard.h"

#define CAPTURE_BIT 0b00000001
#define EN_PASSAN 16
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

struct Move{
  byte from = 0;
  byte to = 0;
  byte flags = 0; //last(leftmost) 4 bits are the ID of the captured piece, if there was a capture
};

int getSquareIndex(int file, int rank);

struct Board{
  u64 bitboards[12];
  byte squares[64];
  byte tags;
  
  void makeMove(Move &m);
  void unmakeMove(Move &m);
  void loadFromFEN(std::string fen);
};