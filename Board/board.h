#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Bitboards/bitboard.h"

#define CAPTURE_BIT       0b00000001
#define EN_PASSAN_NULL    0

#define WHITE_TO_MOVE_BIT    0b00000001
#define WHITE_KINGSIDE_BIT   0b00000010
#define WHITE_QUEENSIDE_BIT  0b00000100
#define BLACK_KINGSIDE_BIT   0b00001000
#define BLACK_QUEENSIDE_BIT  0b00010000
#define THREATENED_POPULATED 0b00100000

#define WHITE_CASTLING_RIGHTS 0b00000110
#define BLACK_CASTLING_RIGHTS 0b00011000

#define CASTLE_KINGSIDE  0b0000000000000001
#define CASTLE_QUEENSIDE 0b0000000000000010
#define EN_PASSAN        0b0000000000000011
#define PROMOTION_BIT    0b0000000000001000
#define PROMOTION_MASK   0b0000000000000111
//masks
#define TO_PIECE_MASK    0b1111110000000000
#define FROM_PIECE_MASK  0b0000001111110000

#define SPECIAL_MOVE_DATA_MASK 0b0000000000001111
#define EN_PASSAN_TARGET_MASK  0b0000111111000000
#define CASTLING_RIGHTS_MASK   0b0000000000111100

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
  unsigned short move = 0; //ttttttffffffssss  t = to f = from s = special move data 
  unsigned short unmakeData = 0;//cccceeeeeerrrr** c = captured e = en passan r = castling rights * = unused 
public:
inline void setTo(byte to) { move |= to<<10;}
inline void setFrom(byte from) { move |= from<<4;}
inline void setSpecialMoveData(byte smd) { move |= smd;}
inline void setPromotion(byte piece) {move |= piece | PROMOTION_BIT;}
inline void setEnPassanTarget(byte target){ unmakeData |= target<<6;}
inline void setCastlingRights(byte flags){unmakeData |= flags<<2;}
inline void setCapturedPiece(byte piece){unmakeData |= piece<<12;}

inline byte getTo() {return (move & TO_PIECE_MASK)>>10;}
inline byte getFrom() {return (move & FROM_PIECE_MASK)>>4;}
inline byte getSpecialMoveData(){ return (move&SPECIAL_MOVE_DATA_MASK);}
inline bool isEnPassan(){ return getSpecialMoveData() == EN_PASSAN;}
inline bool isKingside(){ return getSpecialMoveData() == CASTLE_KINGSIDE;}
inline bool isQueenside(){ return getSpecialMoveData() == CASTLE_QUEENSIDE;}
inline bool isPromotion(){return move & PROMOTION_BIT;}
inline byte getPromotionPiece(){return move & PROMOTION_MASK;}


inline byte getEnPassanTarget(){ return (unmakeData & EN_PASSAN_TARGET_MASK )>>6;}
inline byte getCastlingRights(){return (unmakeData & CASTLING_RIGHTS_MASK)>>2;}
inline byte getCapturedPiece(){return unmakeData>>12;}

unsigned short getUnmakeData(){return unmakeData;}
unsigned short getMoveData(){return move;}
inline void resetUnmakeData(){unmakeData = (unsigned short)0;}
};

int getSquareIndex(int file, int rank);

struct Board{
  static u64 rankMasks[8];
  static u64 fileMasks[8];
  static void generateFileMasks();
  static void generateRankMasks();
  Board(){ generateRankMasks(); generateFileMasks();}
  u64 bitboards[14];
  u64 occupancy; 
  u64 threatened[2]; // squares threatened by white come first
  byte enPassanTarget = EN_PASSAN_NULL;
  byte squares[64];
  byte flags = 0 | WHITE_TO_MOVE_BIT;
  
  void makeMove(Move &m);
  void unmakeMove(Move &m);
  void loadFromFEN(std::string fen);
  void updateColorBitboards();

  bool validate() const;//checks if the position is valid
};
