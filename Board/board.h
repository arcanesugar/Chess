#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Bitboards/bitboard.h"

#define CAPTURE_BIT       0b00000001
#define EN_PASSAN_NULL    0

#define WHITE_TO_MOVE_BIT   0b00000001
#define WHITE_KINGSIDE_BIT  0b00000010
#define WHITE_QUEENSIDE_BIT 0b00000100
#define BLACK_KINGSIDE_BIT  0b00001000
#define BLACK_QUEENSIDE_BIT 0b00010000

#define WHITE_CASTLING_RIGHTS 0b00000110
#define BLACK_CASTLING_RIGHTS 0b00011000

#define CASTLE_KINGSIDE  0b0000000000000001
#define CASTLE_QUEENSIDE 0b0000000000000010
#define EN_PASSAN        0b0000000000000011
//masks
#define TO_PIECE_MASK   0b1111110000000000
#define FROM_PIECE_MASK 0b0000001111110000
#define EN_PASSAN_TARGET_MASK  0b0000111111000000
#define SPECIAL_MOVE_DATA_MASK 0b0000000000000011
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
  unsigned short move = 0; //ttttttffffffrrcc  t = to f = from r = promotion c = castling/en passan
  unsigned short unmakeData = 0; 
  byte boardFlags = 0;

public:
inline void setTo(byte to) { move |= to<<10;}
inline void setFrom(byte from) { move |= from<<4;}
inline void setSpecialMoveData(byte smd) { move |= smd;}

inline void setEnPassanTarget(byte target){ unmakeData |= target<<6;}
inline void setBoardFlags(byte flags){boardFlags = flags;}
inline void setCapturedPiece(byte piece){unmakeData |= piece<<12;}

inline byte getTo() {return (move & TO_PIECE_MASK)>>10;}
inline byte getFrom() {return (move & FROM_PIECE_MASK)>>4;}
inline byte getSpecialMoveData(){ return (move&SPECIAL_MOVE_DATA_MASK);}
inline bool isEnPassan(){ return getSpecialMoveData() == EN_PASSAN;}
inline bool isKingside(){ return getSpecialMoveData() == CASTLE_KINGSIDE;}
inline bool isQueenside(){ return getSpecialMoveData() == CASTLE_QUEENSIDE;}


inline byte getEnPassanTarget(){ return (unmakeData & EN_PASSAN_TARGET_MASK )>>6;}
inline byte getBoardFlags(){return boardFlags;}
inline byte getCapturedPiece(){return unmakeData>>12;}

};

int getSquareIndex(int file, int rank);

struct Board{
  u64 bitboards[14];
  u64 occupancy;//1 if a piece is present
  byte enPassanTarget = EN_PASSAN_NULL;
  byte squares[64];
  byte flags = 0 | WHITE_TO_MOVE_BIT;
  u64 castlingMasks[4] = {1792U,57344U,1970324836974592U,63050394783186944U};
  
  void makeMove(Move &m);
  void unmakeMove(Move &m);
  void loadFromFEN(std::string fen);
  void updateColorBitboards();
};