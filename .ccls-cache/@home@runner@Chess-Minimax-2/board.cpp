#include "board.h"
int getSquareIndex(int file, int rank){return file*8 + (rank);};
void Board::loadFromFEN(std::string fen){
  for(u64 &bb : bitboards){
    bb = (u64)0;
  }
  int squareIndex = 63;
  for(char c: fen){
    if(c == '/') continue;
    if(isdigit(c)){
      for(int i = 0; i<c-'0'; i++){
        squares[squareIndex] = EMPTY;
        squareIndex -=1;
      }
    }else{
      int color = isupper(c) ? WHITE : BLACK;
      int piece = 0;
      switch(std::tolower(c)){
        case 'p': piece = PAWN; break;
        case 'b': piece = BISHOP; break;
        case 'n': piece = KNIGHT; break;
        case 'r': piece = ROOK; break;
        case 'k': piece = KING; break;
        case 'q': piece = QUEEN; break;
      }
      setBit(bitboards[color+piece],squareIndex);
      squares[squareIndex] = color+piece;
      squareIndex-=1;
    }
  }
}

void Board::makeMove(Move &m){
  byte fromPiece = squares[m.from];
  byte toPiece = squares[m.to];
  if(toPiece!=EMPTY) m.flags |= CAPTURE_BIT;
  squares[m.from] = EMPTY;
  squares[m.to] = fromPiece;

  resetBit(bitboards[fromPiece],m.from);
  resetBit(bitboards[toPiece],m.to);
  setBit(bitboards[fromPiece],m.to);
  if(m.flags&CAPTURE_BIT){
    m.flags |= toPiece<<4;
  }
}

void Board::unmakeMove(Move &m){
  byte pieceOnToSquare = squares[m.to];
  //move piece back
  squares[m.from] = squares[m.to];
  setBit(bitboards[pieceOnToSquare],m.from);
  resetBit(bitboards[pieceOnToSquare],m.to);
  
  if(m.flags&CAPTURE_BIT){//undo captures
    setBit(bitboards[m.flags>>4],m.to);
    squares[m.to] = m.flags>>4;
  }else{
    squares[m.to] = EMPTY;
  }

}