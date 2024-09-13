#include <cmath>
#include "board.h"
int getSquareIndex(int rank, int file){return (rank*8) + file;};
void Board::updateColorBitboards(){
  bitboards[WHITE_PIECES] = bitboards[WHITE+PAWN] | bitboards[WHITE+BISHOP] | bitboards[WHITE+KNIGHT] | bitboards[WHITE+ROOK] | bitboards[WHITE+QUEEN] | bitboards[WHITE+KING];
  bitboards[BLACK_PIECES] = bitboards[BLACK+PAWN] | bitboards[BLACK+BISHOP] | bitboards[BLACK+KNIGHT] | bitboards[BLACK+ROOK] | bitboards[BLACK+QUEEN] | bitboards[BLACK+KING];
  occupancy = bitboards[WHITE_PIECES]|bitboards[BLACK_PIECES];
}
void Board::loadFromFEN(std::string fen){
  std::string parsed[6];
  int index = 0;
  for(char c: fen){
    if(c == ' '){
      index++;
      continue;
    }
    parsed[index].push_back(c);
  }
  for(u64 &bb : bitboards){
    bb = (u64)0;
  }
  int squareIndex = 63;
  for(char c: parsed[0]){
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
    if(squareIndex<0){break;}
  }
  updateColorBitboards();

  //set flags
  flags = (u64)0;
  if(parsed[1] == "w"){
    flags |= WHITE_TO_MOVE_BIT;
  }

  if(parsed[2].find("K")) flags |= WHITE_KINGSIDE_BIT;
  if(parsed[2].find("Q")) flags |= WHITE_QUEENSIDE_BIT;
  if(parsed[2].find("k")) flags |= BLACK_KINGSIDE_BIT;
  if(parsed[2].find("q")) flags |= BLACK_QUEENSIDE_BIT;
}

void Board::makeMove(Move &m){
  int color = (flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  if(m.flags&KINGSIDE_BIT){
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    resetBit(bitboards[color+KING],3+offset);
    setBit(bitboards[color+KING],1+offset);
    squares[3+offset] = EMPTY;
    squares[1+offset] = color+KING;
    //move rook
    resetBit(bitboards[WHITE+KING],0+offset);
    setBit(bitboards[WHITE+KING],2+offset);
    squares[0+offset] = EMPTY;
    squares[2+offset] = color+ROOK;

    flags ^= WHITE_TO_MOVE_BIT;
    updateColorBitboards();
    return;
  }
  if(m.flags&QUEENSIDE_BIT){
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    resetBit(bitboards[color+KING],3+offset);
    setBit(bitboards[color+KING],5+offset);
    squares[3+offset] = EMPTY;
    squares[5+offset] = color+KING;
    //move rook
    resetBit(bitboards[WHITE+KING],7+offset);
    setBit(bitboards[WHITE+KING],4+offset);
    squares[7+offset] = EMPTY;
    squares[4+offset] = color+ROOK;

    flags ^= WHITE_TO_MOVE_BIT;
    updateColorBitboards();
    return;
  }
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
  flags ^= WHITE_TO_MOVE_BIT;
  updateColorBitboards();
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
  flags ^= WHITE_TO_MOVE_BIT;
  updateColorBitboards();
}