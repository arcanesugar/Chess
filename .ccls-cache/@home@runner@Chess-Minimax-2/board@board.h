#pragma once
#include "../moves.h"
#include <iostream>
#include <string>

typedef unsigned long long u64;
typedef unsigned char byte;

bool getBit(u64 &bb, int index);
void setBit(u64 &bb, int index);
void resetBit(u64 &bb, int index);

enum piece {
  EMPTY = 255,
  PAWN = 0,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
};

#define BLACK 6
#define WHITE 0

class board {
public:
  u64 bitboards[12] = {0};
  byte squares[64] = {0}; // nibbles later?
  int state = 255; // white kingside, white queenside, black kingside, black
                    // queenside, whiteToMove,
  byte getPiece(int squareIndex) { return squares[squareIndex]; }
  bool whiteToMove() { return (state>>4)&1; }
  void makeMove(Move &m);
  void loadFromFEN(std::string fen);
};