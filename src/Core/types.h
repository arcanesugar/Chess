#pragma once

typedef unsigned long long u64;
typedef unsigned char byte;
typedef char bool;
#define true 1
#define false 0

//WHITE+PAWN = 0, BLACK+BISHOP = 7
#define WHITE 0
#define BLACK 6
#define EMPTY 12

#define WHITE_PIECES 12
#define BLACK_PIECES 13

enum piece{
  PAWN = 0,
  BISHOP,
  KNIGHT,
  ROOK,
  QUEEN,
  KING
};
char getPieceColor(char piece);
void strcatchar(char *string, char c);
