#pragma once
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "Bitboards/bitboard.h"
#include "../types.h"
#define CAPTURE_BIT       0b00000001
#define EN_PASSAN_NULL    0

#define WHITE_TO_MOVE_BIT    0b00000001
#define WHITE_KINGSIDE_BIT   0b00000010
#define WHITE_QUEENSIDE_BIT  0b00000100
#define BLACK_KINGSIDE_BIT   0b00001000
#define BLACK_QUEENSIDE_BIT  0b00010000

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
  unsigned short move = 0; //ttttttffffffssss  t = to f = from s = special move data 
  unsigned short unmakeData = 0;//cccceeeeeerrrr** c = captured e = en passan r = castling rights * = unused 
};

inline void setTo(Move *move, byte to) { move->move |= to<<10;}
inline void setFrom(Move *move, byte from) { move->move |= from<<4;}
inline void setSpecialMoveData(Move *move, byte smd) { move->move |= smd;}
inline void setPromotion(Move *move, byte piece) {move->move |= piece | PROMOTION_BIT;}
inline void setEnPassanTarget(Move *move, byte target){ move->unmakeData |= target<<6;}
inline void setCastlingRights(Move *move, byte flags){move->unmakeData |= flags<<2;}
inline void setCapturedPiece(Move *move, byte piece){move->unmakeData |= piece<<12;}

inline byte getTo(Move *move) {return (move->move & TO_PIECE_MASK)>>10;}
inline byte getFrom(Move *move) {return (move->move & FROM_PIECE_MASK)>>4;}
inline byte getSpecialMoveData(Move *move){ return (move->move&SPECIAL_MOVE_DATA_MASK);}
inline bool isEnPassan(Move *move){ return getSpecialMoveData(move) == EN_PASSAN;}
inline bool isKingside(Move *move){ return getSpecialMoveData(move) == CASTLE_KINGSIDE;}
inline bool isQueenside(Move *move){ return getSpecialMoveData(move) == CASTLE_QUEENSIDE;}
inline bool isPromotion(Move *move){return move->move & PROMOTION_BIT;}
inline byte getPromotionPiece(Move *move){return move->move & PROMOTION_MASK;}

inline byte getEnPassanTarget(Move *move){return (move->unmakeData & EN_PASSAN_TARGET_MASK )>>6;}
inline byte getCastlingRights(Move *move){return (move->unmakeData & CASTLING_RIGHTS_MASK)>>2;}
inline byte getCapturedPiece(Move *move) {return move->unmakeData>>12;}

inline void resetUnmakeData(Move *move){move->unmakeData = (unsigned short)0;}

int getSquareIndex(int file, int rank);

extern u64 rankMasks[8];
extern u64 fileMasks[8];
void generateBoardMasks();

struct Board{
  u64 bitboards[14];
  u64 occupancy; 
  byte enPassanTarget = EN_PASSAN_NULL;
  byte squares[64];
  byte flags = 0 | WHITE_TO_MOVE_BIT;
};

Board boardFromFEN(char* fen);
bool validateBoard(Board board);

void makeMove(Board *board, Move *m);
void unmakeMove(Board *board, Move *m);
