#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Bitboards/bitboard.h"

#define CAPTURE_BIT       0b00000001
#define KINGSIDE_BIT      0b00000010
#define QUEENSIDE_BIT     0b00000100
#define EN_PASSAN_BIT     0b00001000
#define EN_PASSAN_NULL    0

#define WHITE_TO_MOVE_BIT   0b00000001
#define WHITE_KINGSIDE_BIT  0b00000010
#define WHITE_QUEENSIDE_BIT 0b00000100
#define BLACK_KINGSIDE_BIT  0b00001000
#define BLACK_QUEENSIDE_BIT 0b00010000

#define WHITE_CASTLING_RIGHTS 0b00000110
#define BLACK_CASTLING_RIGHTS 0b00011000


//masks
#define TO_PIECE_MASK   0b1111110000000000
#define FROM_PIECE_MASK 0b0000001111110000
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
private:
  //unsigned short move = 0; //ttttttffffffrrcc  t = to f = from r = promotion c = castling
byte to = 0;
byte from = 0;
  byte enPassanTarget = EN_PASSAN_NULL; //the en passan target before the move was made
  byte boardFlags = 0;

public:
byte flags = 0; //last(leftmost) 4 bits are the ID of the captured piece, if there was a capture
inline void setTo(byte to) { /*move |= to<<10;*/ this->to = to;}
inline void setFrom(byte from) { /*move |= from<<4;*/ this->from = from;}
inline void setBoardFlags(byte flags){boardFlags = flags;}
inline void setEnPassanTarget(byte target){ enPassanTarget = target;}
inline void setCapturedPiece(byte piece){flags |= piece<<4;}

inline byte getTo() {return to; /*return (move & TO_PIECE_MASK)>>10;*/}
inline byte getFrom() {return from; /*return (move & FROM_PIECE_MASK)>>4;*/}
inline byte getEnPassanTarget(){ return enPassanTarget;}
inline byte getBoardFlags(){return boardFlags;}
inline byte getCapturedPiece(){return flags>>4;}

};

int getSquareIndex(int file, int rank);

struct Board{
  u64 bitboards[14];
  u64 occupancy;//1 if a piece is present
  byte enPassanTarget = 255;
  byte squares[64];
  byte flags = 0 | WHITE_TO_MOVE_BIT;
  u64 castlingMasks[4] = {1792U,57344U,1970324836974592U,63050394783186944U};
  
  void makeMove(Move &m);
  void unmakeMove(Move &m);
  void loadFromFEN(std::string fen);
  void updateColorBitboards();
};