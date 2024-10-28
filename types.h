#pragma once
typedef unsigned long long u64;
typedef unsigned char byte;

//typedef char bool;
//#define true 1;
//#define false 0;

void strcatchar(char *string, char c);

//Move is completely abstracted, never access move or unmakeData directly
struct Move{
  unsigned short move; //ttttttffffffssss  t = to f = from s = special move data 
  unsigned short unmakeData;//cccceeeeeerrrr** c = captured e = en passan r = castling rights * = unused 
};

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
inline Move createNullMove(){return Move{0};}
inline Move createEmptyMove(){return Move{0};}
inline bool isNullMove(Move* move){return !(move->move|move->unmakeData);}


struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is just a beter number(and adds room for psedeo legal moves)
  byte end;
};

MoveList createMoveList();
void moveListAppend(MoveList *ml, Move m);
void moveListRemove(MoveList *ml, byte index);//very slow

//functions to treat MoveList like a stack