#include "board.h"

bool getBit(u64 bb, int index){
  return (bb>>index)&1;
}
void setBit(u64 &bb, int index){
  u64 mask = 1<<index;
  bb |= mask;
}

void board::loadFromFEN(std::string fen){
  int squareIndex = 63;
  for(int i = 0; i<fen.length(); i++){
    if(isdigit(fen[i])){
      for(int j = 0; j<(fen[i] - '0'); j++){
        squares[squareIndex] = EMPTY;
        squareIndex-= 1;
      }
    }else{
      if(fen[i] == '/') continue;
      byte piece = 0;
      if(islower(fen[i])){
        piece += BLACK;
      }
      switch(tolower(fen[i])){
        case 'p': {piece += PAWN; break;}
        case 'r': {piece += ROOK; break;}
        case 'b': {piece += BISHOP; break;}
        case 'n': {piece += KNIGHT; break;}
        case 'k': {piece += KING; break;}
        case 'q': {piece += QUEEN; break;}
      }
      setBit(bitboards[piece],squareIndex);
      squares[squareIndex] = piece;
      squareIndex -=1;
    }
  }
}