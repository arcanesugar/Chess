#include "types.h"
#include <string.h>

class test{
public:
  int hi;
};
void strcatchar(char *string, char c){
  char temp[2] = " ";
  temp[0] = c;
  strcat(string,temp);
}

//Move List
struct MoveList createMoveList(){
  struct MoveList ml; ml.end = 0; return ml;
}
void moveListAppend(struct MoveList *ml, Move m){
  ml->moves[ml->end] = m;
  ml->end++;
}
void moveListRemove(struct MoveList *ml, byte index){
  for (byte i = index; i < ml->end; i++)
    ml->moves[i] = ml->moves[i + 1]; // copy next element left
  ml->end-=1;
}

void setTo(Move *move, byte to) { move->move |= to<<10;}
void setFrom(Move *move, byte from) { move->move |= from<<4;}
void setSpecialMoveData(Move *move, byte smd) { move->move |= smd;}
void setPromotion(Move *move, byte piece) {move->move |= piece | PROMOTION_BIT;}
void setEnPassanTarget(Move *move, byte target){ move->unmakeData |= target<<6;}
void setCastlingRights(Move *move, byte flags){move->unmakeData |= flags<<2;}
void setCapturedPiece(Move *move, byte piece){move->unmakeData |= piece<<12;}

byte getTo(Move *move) {return (move->move & TO_PIECE_MASK)>>10;}
byte getFrom(Move *move) {return (move->move & FROM_PIECE_MASK)>>4;}
byte getSpecialMoveData(Move *move){ return (move->move&SPECIAL_MOVE_DATA_MASK);}
bool isEnPassan(Move *move){ return getSpecialMoveData(move) == EN_PASSAN;}
bool isKingside(Move *move){ return getSpecialMoveData(move) == CASTLE_KINGSIDE;}
bool isQueenside(Move *move){ return getSpecialMoveData(move) == CASTLE_QUEENSIDE;}
bool isPromotion(Move *move){return move->move & PROMOTION_BIT;}
byte getPromotionPiece(Move *move){return move->move & PROMOTION_MASK;}

byte getEnPassanTarget(Move *move){return (move->unmakeData & EN_PASSAN_TARGET_MASK )>>6;}
byte getCastlingRights(Move *move){return (move->unmakeData & CASTLING_RIGHTS_MASK)>>2;}
byte getCapturedPiece(Move *move) {return move->unmakeData>>12;}

void resetUnmakeData(Move *move){move->unmakeData = (unsigned short)0;}
Move createNullMove(){Move m; m.move = 0; m.unmakeData = 0; return m;}
Move createEmptyMove(){Move m; m.move = 0; m.unmakeData = 0; return m;}
bool isNullMove(Move* move){return !(move->move|move->unmakeData);}
