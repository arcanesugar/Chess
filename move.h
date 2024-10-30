#pragma once

#include "types.h"

//predefined special move data values
#define CASTLE_KINGSIDE  0b0000000000000001
#define CASTLE_QUEENSIDE 0b0000000000000010
#define EN_PASSAN        0b0000000000000011

//Move is completely abstracted, never access move or unmakeData directly
//set functions only work if that proporty has not yet been set, and the initial move was empty
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

void resetUnmakeData(Move *move);
Move createNullMove();
Move createEmptyMove();
bool isNullMove(Move* move);

void moveToString(Move move, char *target);

struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is just a beter number(and adds room for psedeo legal moves)
  byte end;
};

struct MoveList createMoveList();
void moveListAppend(struct MoveList *ml, Move m);
void moveListRemove(struct MoveList *ml, byte index);//very slow
