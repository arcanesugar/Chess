#pragma once

#include "types.h"

//predefined special move data values
#define CASTLE_KINGSIDE  0b0000000000000001
#define CASTLE_QUEENSIDE 0b0000000000000010
#define EN_PASSAN        0b0000000000000011

//Set functions only work if that property has not yet been set, and the initial move was empty
struct Move{
  unsigned short move; //ttttttffffffssss  t = to f = from s = special move data 
  unsigned short unmakeData;//cccceeeeeerrrr** c = captured e = en passan r = castling rights * = unused 
};
typedef struct Move Move;

void setTo(Move *move, byte to);
void setFrom(Move *move, byte from);
void setSpecialMoveData(Move *move, byte smd);
void setPromotion(Move *move, byte piece);
void setEnPassanTarget(Move *move, byte target);
void setCastlingRights(Move *move, byte flags);
void setCapturedPiece(Move *move, byte piece);

byte getTo(Move *move);
byte getFrom(Move *move);
byte getSpecialMoveData(Move *move);
bool isEnPassan(Move *move);
bool isKingside(Move *move);
bool isQueenside(Move *move);
bool isPromotion(Move *move);
byte getPromotionPiece(Move *move);

byte getEnPassanTarget(Move *move);
byte getCastlingRights(Move *move);
byte getCapturedPiece(Move *move);

Move createNullMove();
Move createEmptyMove();
bool isNullMove(Move* move);

void moveToString(Move move, char *target, char sideToMove);

#define MOVE_LIST_MAX 219
struct MoveList{
  Move moves[MOVE_LIST_MAX];
  byte end;
};
typedef struct MoveList MoveList;

MoveList createMoveList();
void moveListAppend(MoveList *ml, Move m);
void moveListRemove(MoveList *ml, byte index);//very slow
bool moveListEmpty(MoveList ml);
void moveListPop(MoveList *ml);
