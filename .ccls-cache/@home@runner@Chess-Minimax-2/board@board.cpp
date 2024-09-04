#include "board.h"

bool getBit(u64 &bb, int index) { return (bb >> index) & (u64)1; }
void setBit(u64 &bb, int index) { bb |= (u64)1 << index; }
void resetBit(u64 &bb, int index) { bb &= ~((u64)1 << index); }

void board::makeMove(Move &m) {
  //toggle white to move
  if(whiteToMove()){
    state &= ~(1<<4);
  }else{
    state |= 1<<4;
  }

  byte piece = squares[m.from];
  squares[m.from] = EMPTY;
  squares[m.to] = piece;
  resetBit(bitboards[piece],m.from);
  setBit(bitboards[piece],m.to);
}

void board::loadFromFEN(std::string fen) { // disgusting mess but functional
  int col = 0;                             // x
  int row = 7;                             // y
  for (int i = 0; i < fen.length(); i++) {
    if (isdigit(fen[i])) {
      for (int j = 0; j < (fen[i] - '0'); j++) {
        squares[(row * 8) + col] = EMPTY;
        col += 1;
      }
    } else {
      if (fen[i] == '/') {
        row -= 1;
        col = 0;
        continue;
      };
      byte piece = 0;
      if (islower(fen[i])) {
        piece += BLACK;
      }
      switch (tolower(fen[i])) {
      case 'p': {
        piece += PAWN;
        break;
      }
      case 'r': {
        piece += ROOK;
        break;
      }
      case 'b': {
        piece += BISHOP;
        break;
      }
      case 'n': {
        piece += KNIGHT;
        break;
      }
      case 'k': {
        piece += KING;
        break;
      }
      case 'q': {
        piece += QUEEN;
        break;
      }
      }
      setBit(bitboards[piece], (row * 8) + col);
      squares[(row * 8) + col] = piece;
      col += 1;
    }
  }
}