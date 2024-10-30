#include <string.h>

#include "move.h"

//masks for move.move
#define TO_PIECE_MASK          0b1111110000000000
#define FROM_PIECE_MASK        0b0000001111110000
#define SPECIAL_MOVE_DATA_MASK 0b0000000000001111
#define PROMOTION_MASK         0b0000000000000111
#define PROMOTION_BIT          0b0000000000001000

//masks for move.unmakeData
#define EN_PASSAN_TARGET_MASK  0b0000111111000000
#define CASTLING_RIGHTS_MASK   0b0000000000111100




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

Move createNullMove(){Move m; m.move = 0; m.unmakeData = 0; return m;}
Move createEmptyMove(){Move m; m.move = 0; m.unmakeData = 0; return m;}
bool isNullMove(Move* move){return !(move->move|move->unmakeData);}

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

void moveToString(Move move, char *target){
  if(isKingside(&move)){
    strcpy(target, "ks");
    return;
  }
  if(isQueenside(&move)){
    strcpy(target, "qs");
    return;
  }
  char fromStr[3] = "  ";
  char toStr[3] = "  ";
  fromStr[0] = 'h'-(getFrom(&move)%8);
  toStr[0] = 'h'-(getTo(&move)%8);
  fromStr[1] = '1'+(getFrom(&move)/8);
  toStr[1] = '1'+(getTo(&move)/8);
  
  strcpy(target, "[");
  strcat(target,fromStr);
  strcat(target, "->");
  strcat(target,toStr);
  strcat(target, "]");
  if(isPromotion(&move)){
    switch(getPromotionPiece(&move)%6){
      case BISHOP:
        strcat(target,"b");
      break;
      case ROOK:
        strcat(target,"r");
      break;
      case KNIGHT:
        strcat(target,"n");
      break;
      case QUEEN:
        strcat(target,"q");
      break;
    }
  }
}


