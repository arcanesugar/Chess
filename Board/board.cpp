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
  for(byte &b : squares){
    b = EMPTY;
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
  if(parsed[1] == "w")
    flags |= WHITE_TO_MOVE_BIT;

  if(parsed[2].find("K") != parsed[2].npos) flags |= WHITE_KINGSIDE_BIT;
  if(parsed[2].find("Q") != parsed[2].npos) flags |= WHITE_QUEENSIDE_BIT;
  if(parsed[2].find("k") != parsed[2].npos) flags |= BLACK_KINGSIDE_BIT;
  if(parsed[2].find("q") != parsed[2].npos) flags |= BLACK_QUEENSIDE_BIT;
}

void Board::makeMove(Move &m){
  int color = (flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  m.setBoardFlags(flags);
  if(m.flags&KINGSIDE_BIT){
    //this side can no longer castle
    if(color == WHITE) flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    resetBit(bitboards[color+KING],3+offset);
    setBit(bitboards[color+KING],1+offset);
    squares[3+offset] = EMPTY;
    squares[1+offset] = color+KING;
    //move rook
    resetBit(bitboards[color+ROOK],0+offset);
    setBit(bitboards[color+ROOK],2+offset);
    squares[0+offset] = EMPTY;
    squares[2+offset] = color+ROOK;

    flags ^= WHITE_TO_MOVE_BIT;
    updateColorBitboards();
    return;
  }
  if(m.flags&QUEENSIDE_BIT){
    //this side can no longer castle
    if(color == WHITE) flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    resetBit(bitboards[color+KING],3+offset);
    setBit(bitboards[color+KING],5+offset);
    squares[3+offset] = EMPTY;
    squares[5+offset] = color+KING;
    //move rook
    resetBit(bitboards[color+ROOK],7+offset);
    setBit(bitboards[color+ROOK],4+offset);
    squares[7+offset] = EMPTY;
    squares[4+offset] = color+ROOK;

    flags ^= WHITE_TO_MOVE_BIT;
    updateColorBitboards();
    return;
  }

  byte to = m.getTo();
  byte from = m.getFrom();
  byte fromPiece = squares[from];
  byte toPiece = squares[to];
  squares[from] = EMPTY;
  squares[to] = fromPiece;

  resetBit(bitboards[fromPiece],from);
  resetBit(bitboards[toPiece],to);
  setBit(bitboards[fromPiece],to);
  m.setCapturedPiece(toPiece);//store captured piece (if there is no piece it will just be empty)
  
  if(fromPiece == color+KING){
    if(flags&WHITE_TO_MOVE_BIT){
      flags &= ~(WHITE_CASTLING_RIGHTS);
    }else{
      flags &= ~(BLACK_CASTLING_RIGHTS);
    }
  }
  
  //update castling rights
  byte rookSquares[4] = {0,7,56,63};
  for(int i = 0; i<4; i++){
    if(from == rookSquares[i] || to == rookSquares[i]){
      flags &= ~(WHITE_KINGSIDE_BIT<<i);
    }
    if((u64(1)<<fromPiece) & castlingMasks[i]){
      flags &= ~(WHITE_KINGSIDE_BIT<<i);
    }
  }

  //do en passan
  if(m.flags & EN_PASSAN_BIT){
    if(flags&WHITE_TO_MOVE_BIT){
      resetBit(bitboards[BLACK+PAWN],to-8);
      squares[to-8] = EMPTY;
    }else{
      resetBit(bitboards[WHITE+PAWN],to+8);
      squares[to+8] = EMPTY;
    }
  }
  
  //Update en passan target
  m.setEnPassanTarget(enPassanTarget);
  enPassanTarget = EN_PASSAN_NULL;
  if(fromPiece == PAWN || fromPiece == BLACK+PAWN){
    if(std::abs(to-from)>9){//double forward move
      enPassanTarget = to;
      if(flags&WHITE_TO_MOVE_BIT){
        enPassanTarget -= 8;
      }else{
        enPassanTarget +=8;
      }
    }
  }

  flags ^= WHITE_TO_MOVE_BIT;
  updateColorBitboards();
}

void Board::unmakeMove(Move &m){
  int color = (flags & WHITE_TO_MOVE_BIT) ? BLACK : WHITE;
  if(m.flags&KINGSIDE_BIT){
    flags ^= WHITE_TO_MOVE_BIT;
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    setBit(bitboards[color+KING],3+offset);
    resetBit(bitboards[color+KING],1+offset);
    squares[1+offset] = EMPTY;
    squares[3+offset] = color+KING;
    //move rook
    setBit(bitboards[color+ROOK],0+offset);
    resetBit(bitboards[color+ROOK],2+offset);
    squares[2+offset] = EMPTY;
    squares[0+offset] = color+ROOK;

    enPassanTarget = m.getEnPassanTarget();
    flags  = m.getBoardFlags();
    updateColorBitboards();
    return;
  }
  if(m.flags&QUEENSIDE_BIT){
    flags ^= WHITE_TO_MOVE_BIT;
    int offset= (flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    //move king
    setBit(bitboards[color+KING],3+offset);
    resetBit(bitboards[color+KING],5+offset);
    squares[5+offset] = EMPTY;
    squares[3+offset] = color+KING;
    //move rook
    setBit(bitboards[color+ROOK],7+offset);
    resetBit(bitboards[color+ROOK],4+offset);
    squares[4+offset] = EMPTY;
    squares[7+offset] = color+ROOK;

    enPassanTarget = m.getEnPassanTarget();
    flags  = m.getBoardFlags();
    updateColorBitboards();
    return;
  }
  
  //move piece back
  byte to = m.getTo();
  byte from = m.getFrom();
  
  byte pieceOnToSquare = squares[to];
  squares[from] = squares[to];
  setBit(bitboards[pieceOnToSquare],from);
  resetBit(bitboards[pieceOnToSquare],to);
  setBit(bitboards[m.flags>>4],to);
  squares[to] = m.getCapturedPiece();

  //undo en passan
  if(m.flags & EN_PASSAN_BIT){
    if(flags&WHITE_TO_MOVE_BIT){
      setBit(bitboards[WHITE+PAWN],to+8);
      squares[to+8] = WHITE+PAWN;
    }else{
      setBit(bitboards[BLACK+PAWN],to-8);
      squares[to-8] = BLACK+PAWN;
    }
  }

  //restore board state
  enPassanTarget = m.getEnPassanTarget();
  flags  = m.getBoardFlags();
  updateColorBitboards();
}