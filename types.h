#pragma once
typedef unsigned long long u64;
typedef unsigned char byte;
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __cplusplus
typedef char bool;
#define true 1;
#define false 0;
#endif

typedef unsigned long long u64;
void strcatchar(char *string, char c);

//Move is completely abstracted, never access move or unmakeData directly
struct Move{
  unsigned short move; //ttttttffffffssss  t = to f = from s = special move data 
  unsigned short unmakeData;//cccceeeeeerrrr** c = captured e = en passan r = castling rights * = unused 
};
typedef struct Move Move;

//masks for move.move
#define TO_PIECE_MASK          0b1111110000000000
#define FROM_PIECE_MASK        0b0000001111110000
#define SPECIAL_MOVE_DATA_MASK 0b0000000000001111
#define PROMOTION_MASK         0b0000000000000111
#define PROMOTION_BIT          0b0000000000001000

//masks for move.unmakeData
#define EN_PASSAN_TARGET_MASK  0b0000111111000000
#define CASTLING_RIGHTS_MASK   0b0000000000111100

//predefined special move data values
#define CASTLE_KINGSIDE  0b0000000000000001
#define CASTLE_QUEENSIDE 0b0000000000000010
#define EN_PASSAN        0b0000000000000011



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


struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is just a beter number(and adds room for psedeo legal moves)
  byte end;
};

struct MoveList createMoveList();
void moveListAppend(struct MoveList *ml, Move m);
void moveListRemove(struct MoveList *ml, byte index);//very slow

//functions to treat MoveList like a stack
#ifdef __cplusplus
}
#endif
