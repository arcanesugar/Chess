#include <cmath>
#include "board.h"

u64 Board::rankMasks[8];
u64 Board::fileMasks[8];

int getSquareIndex(int rank, int file){return (rank*8) + file;};
void Board::generateRankMasks() {
  for (int i = 0; i < 8; i++) {
    rankMasks[i] = (u64)255 << (8 * i);
  }
}
void Board::generateFileMasks() {
  for (int i = 0; i < 8; i++) {
    u64 mask = (u64)0;
    for (int y = 0; y < 8; y++) 
      setBit(mask, (y * 8) + i);
    fileMasks[i] = mask;
  }
}
void Board::updateColorBitboards(){
  Board &board = *this;//gross but temporary
  board.bitboards[WHITE_PIECES] = board.bitboards[WHITE+PAWN] | board.bitboards[WHITE+BISHOP] | board.bitboards[WHITE+KNIGHT] | board.bitboards[WHITE+ROOK] | board.bitboards[WHITE+QUEEN] | board.bitboards[WHITE+KING];
  board.bitboards[BLACK_PIECES] = board.bitboards[BLACK+PAWN] | board.bitboards[BLACK+BISHOP] | board.bitboards[BLACK+KNIGHT] | board.bitboards[BLACK+ROOK] | board.bitboards[BLACK+QUEEN] | board.bitboards[BLACK+KING];
  occupancy = board.bitboards[WHITE_PIECES]|board.bitboards[BLACK_PIECES];
}
bool Board::validate() const{//Way too expensive to use ouside of debugging
  if(bitScanForward(bitboards[WHITE+KING]) == -1) return false;
  if(bitScanForward(bitboards[BLACK+KING]) == -1) return false;
  if(enPassanTarget == 1) return false;
  int white = 0;
  u64 whitebb = bitboards[WHITE+PAWN];
  while(whitebb){
    white++;
    popls1b(whitebb);
  }
  int black = 0;
  u64 blackbb = bitboards[BLACK+PAWN];
  while(blackbb){
    black++;
    popls1b(blackbb);
  }
  if(white>8) return false;
  if(black>8) return false;

  //make sure piecewise lookup table is correct
  for(int i = 0; i <64; i++){
    for(int j = 0; j<= BLACK+KING;j++){
      if(getBit(bitboards[j],i)){
        if(squares[i] != j) return false;
      }
    }
  }
  return true;
}
void Board::loadFromFEN(std::string fen){
  Board &board = *this;
  std::string parsed[6];
  int index = 0;
  for(char c: fen){
    if(c == ' '){
      index++;
      continue;
    }
    parsed[index].push_back(c);
  }
  for(u64 &bb : board.bitboards){
    bb = (u64)0;
  }
  for(byte &b : board.squares){
    b = EMPTY;
  }
  int squareIndex = 63;
  for(char c: parsed[0]){
    if(c == '/') continue;
    if(isdigit(c)){
      for(int i = 0; i<c-'0'; i++){
        board.squares[squareIndex] = EMPTY;
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
      setBit(board.bitboards[color+piece],squareIndex);
      board.squares[squareIndex] = color+piece;
      squareIndex-=1;
    }
    if(squareIndex<0){break;}
  }
  updateColorBitboards();

  //set board.flags
  board.flags = (u64)0;
  if(parsed[1] == "w")
    board.flags |= WHITE_TO_MOVE_BIT;

  if(parsed[2].find("K") != parsed[2].npos) board.flags |= WHITE_KINGSIDE_BIT;
  if(parsed[2].find("Q") != parsed[2].npos) board.flags |= WHITE_QUEENSIDE_BIT;
  if(parsed[2].find("k") != parsed[2].npos) board.flags |= BLACK_KINGSIDE_BIT;
  if(parsed[2].find("q") != parsed[2].npos) board.flags |= BLACK_QUEENSIDE_BIT;
  enPassanTarget = EN_PASSAN_NULL;
  if(!std::isdigit(parsed[3][0])){
    if(parsed[3] != "-"){
      std::cout<<"En passan target from fen not yet implemented"<<std::endl;
    }
  }
}
void Board::movePiece(byte fromSquare, byte toSquare, byte type, byte capturedPiece){
  Board &board = *this;
  board.squares[fromSquare] = EMPTY;
  board.squares[toSquare] = type;

  resetBit(board.bitboards[type],fromSquare);
  setBit(board.bitboards[type],toSquare);
  resetBit(board.bitboards[capturedPiece],toSquare);
}
void makeMove(Board &board, Move &m){
  int color = (board.flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  m.setCastlingRights((board.flags&(0b00011110))>>1);
  m.setEnPassanTarget(board.enPassanTarget);
  board.enPassanTarget = EN_PASSAN_NULL;
  if(m.isKingside()){
    //this side can no longer castle
    if(color == WHITE) board.flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) board.flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    board.movePiece(3+offset, 1+offset, color+KING, EMPTY);
    board.movePiece(0+offset, 2+offset, color+ROOK, EMPTY);

    board.flags ^= WHITE_TO_MOVE_BIT;
    board.updateColorBitboards();
    return;
  }
  if(m.isQueenside()){
    //this side can no longer castle
    if(color == WHITE) board.flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) board.flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    board.movePiece(3+offset, 5+offset, color+KING, EMPTY);
    board.movePiece(7+offset, 4+offset, color+ROOK, EMPTY);

    board.flags ^= WHITE_TO_MOVE_BIT;
    board.updateColorBitboards();
    return;
  }

  byte to = m.getTo();
  byte from = m.getFrom();
  if(from < color){
    //std::cout<<"WRONG COLOR\n";
  }
  if(m.isPromotion()){
    resetBit(board.bitboards[board.squares[from]], from);
    setBit(board.bitboards[color+m.getPromotionPiece()],from);
    board.squares[from] = color+m.getPromotionPiece();
  }
  byte fromPiece = board.squares[from];
  byte toPiece = board.squares[to];
  board.movePiece(from, to, fromPiece, toPiece);
  m.setCapturedPiece(toPiece);//store captured piece (if there is no piece it will just be empty)
  
  if(fromPiece == color+KING){
    if(board.flags&WHITE_TO_MOVE_BIT){
      board.flags &= ~(WHITE_CASTLING_RIGHTS);
    }else{
      board.flags &= ~(BLACK_CASTLING_RIGHTS);
    }
  }
  
  //update castling rights
  byte rookSquares[4] = {0,7,56,63};
  for(int i = 0; i<4; i++){
    if(from == rookSquares[i] || to == rookSquares[i]){
      board.flags &= ~(WHITE_KINGSIDE_BIT<<i);
    }
  }

  //do en passan
  if(m.isEnPassan()){
    if(board.flags&WHITE_TO_MOVE_BIT){
      resetBit(board.bitboards[BLACK+PAWN],to-8);
      board.squares[to-8] = EMPTY;
    }else{
      resetBit(board.bitboards[WHITE+PAWN],to+8);
      board.squares[to+8] = EMPTY;
    }
  }
  
  //Update en passan target
  if(fromPiece == PAWN || fromPiece == BLACK+PAWN){
    if(std::abs(to-from)>9){//double forward move
      board.enPassanTarget = to;
      if(board.flags&WHITE_TO_MOVE_BIT){
        board.enPassanTarget -= 8;
      }else{
        board.enPassanTarget +=8;
      }
    }
  }

  board.flags ^= WHITE_TO_MOVE_BIT;
  board.updateColorBitboards();
}

void unmakeMove(Board &board, Move &m){
  int color = (board.flags & WHITE_TO_MOVE_BIT) ? BLACK : WHITE;
  board.enPassanTarget = m.getEnPassanTarget();
  if(m.isKingside()){
    board.flags ^= WHITE_TO_MOVE_BIT;
    
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    board.movePiece(1+offset, 3+offset, color+KING, EMPTY);
    board.movePiece(2+offset, 0+offset, color+ROOK, EMPTY);

    board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board.flags  |= m.getCastlingRights()<<1;
    board.updateColorBitboards();
    return;
  }
  if(m.isQueenside()){
    board.flags ^= WHITE_TO_MOVE_BIT;
    
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    board.movePiece(5+offset, 3+offset, color+KING, EMPTY);
    board.movePiece(4+offset, 7+offset, color+ROOK, EMPTY);

    board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board.flags  |= m.getCastlingRights()<<1;
    board.updateColorBitboards();
    return;
  }

  byte to = m.getTo();
  byte from = m.getFrom();

  //if piece was promoted, turn it back to a pawn
  if(m.isPromotion()){
    resetBit(board.bitboards[board.squares[to]], to);
    setBit(board.bitboards[PAWN+color],to);
    board.squares[to] = PAWN + color; 
  }
  
  //move piece back
  byte pieceOnToSquare = board.squares[to];
  board.squares[from] = board.squares[to];
  setBit(board.bitboards[pieceOnToSquare],from);
  resetBit(board.bitboards[pieceOnToSquare],to);
  setBit(board.bitboards[m.getCapturedPiece()],to);
  board.squares[to] = m.getCapturedPiece();

  //undo en passan
  if(m.isEnPassan()){
    if(board.flags&WHITE_TO_MOVE_BIT){
      setBit(board.bitboards[WHITE+PAWN],to+8);
      board.squares[to+8] = WHITE+PAWN;
    }else{
      setBit(board.bitboards[BLACK+PAWN],to-8);
      board.squares[to-8] = BLACK+PAWN;
    }
  }

  //restore board state
  board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
  board.flags  |= m.getCastlingRights()<<1;
  board.flags ^= WHITE_TO_MOVE_BIT;
  board.updateColorBitboards();
}
