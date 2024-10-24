#include "board.h"

u64 rankMasks[8];
u64 fileMasks[8];

int getSquareIndex(int rank, int file){return (rank*8) + file;};


void generateRankMasks() {
  for (int i = 0; i < 8; i++) {
    rankMasks[i] = (u64)255 << (8 * i);
  }
}
void generateFileMasks() {
  for (int i = 0; i < 8; i++) {
    u64 mask = (u64)0;
    for (int y = 0; y < 8; y++) 
      setBit(mask, (y * 8) + i);
    fileMasks[i] = mask;
  }
}

void generateBoardMasks(){
  generateRankMasks();
  generateFileMasks();
}

bool validateBoard(Board board) {//Way too expensive to use ouside of debugging
  if(bitScanForward(board.bitboards[WHITE+KING]) == -1) return false;
  if(bitScanForward(board.bitboards[BLACK+KING]) == -1) return false;
  if(board.enPassanTarget == 1) return false;
  int white = 0;
  u64 whitebb = board.bitboards[WHITE+PAWN];
  while(whitebb){
    white++;
    popls1b(whitebb);
  }
  int black = 0;
  u64 blackbb = board.bitboards[BLACK+PAWN];
  while(blackbb){
    black++;
    popls1b(blackbb);
  }
  if(white>8) return false;
  if(black>8) return false;

  //make sure piecewise lookup table is correct
  for(int i = 0; i <64; i++){
    for(int j = 0; j<= BLACK+KING;j++){
      if(getBit(board.bitboards[j],i)){
        if(board.squares[i] != j) return false;
      }
    }
  }
  return true;
}
Board boardFromFEN(std::string fen){
  Board board;
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
  board.occupancy = (u64)0;
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
      setBit(board.occupancy, squareIndex);
      if(color == WHITE)
        setBit(board.bitboards[WHITE_PIECES],squareIndex);
      else
        setBit(board.bitboards[BLACK_PIECES],squareIndex);
      board.squares[squareIndex] = color+piece;
      squareIndex-=1;
    }
    if(squareIndex<0){break;}
  }
  //set board.flags
  board.flags = (u64)0;
  if(parsed[1] == "w")
    board.flags |= WHITE_TO_MOVE_BIT;

  if(parsed[2].find("K") != parsed[2].npos) board.flags |= WHITE_KINGSIDE_BIT;
  if(parsed[2].find("Q") != parsed[2].npos) board.flags |= WHITE_QUEENSIDE_BIT;
  if(parsed[2].find("k") != parsed[2].npos) board.flags |= BLACK_KINGSIDE_BIT;
  if(parsed[2].find("q") != parsed[2].npos) board.flags |= BLACK_QUEENSIDE_BIT;
  board.enPassanTarget = EN_PASSAN_NULL;
  if(!std::isdigit(parsed[3][0])){
    if(parsed[3] != "-"){
      printf("En passan target from fen not yet implemented");
    }
  }
  return board;
}

void movePiece(Board &board, byte fromSquare, byte toSquare, byte type, byte capturedPiece){
  board.squares[fromSquare] = EMPTY;
  board.squares[toSquare] = type;

  resetBit(board.bitboards[type],fromSquare);
  setBit(board.bitboards[type],toSquare);
  resetBit(board.bitboards[capturedPiece],toSquare);

  if(type>=BLACK){
    resetBit(board.bitboards[BLACK_PIECES], fromSquare);
    setBit(board.bitboards[BLACK_PIECES], toSquare);
    resetBit(board.bitboards[WHITE_PIECES], toSquare);
  }else{
    resetBit(board.bitboards[WHITE_PIECES], fromSquare);
    setBit(board.bitboards[WHITE_PIECES], toSquare);
    resetBit(board.bitboards[BLACK_PIECES], toSquare);
  }
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
    movePiece(board, 3+offset, 1+offset, color+KING, EMPTY);
    movePiece(board, 0+offset, 2+offset, color+ROOK, EMPTY);

    board.flags ^= WHITE_TO_MOVE_BIT;
    board.occupancy = board.bitboards[WHITE_PIECES] | board.bitboards[BLACK_PIECES];
    return;
  }
  if(m.isQueenside()){
    //this side can no longer castle
    if(color == WHITE) board.flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) board.flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    movePiece(board, 3+offset, 5+offset, color+KING, EMPTY);
    movePiece(board, 7+offset, 4+offset, color+ROOK, EMPTY);

    board.flags ^= WHITE_TO_MOVE_BIT;
    board.occupancy = board.bitboards[WHITE_PIECES] | board.bitboards[BLACK_PIECES];
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
  movePiece(board, from, to, fromPiece, toPiece);
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
      resetBit(board.bitboards[BLACK_PIECES],to-8);
      board.squares[to-8] = EMPTY;
    }else{
      resetBit(board.bitboards[WHITE+PAWN],to+8);
      resetBit(board.bitboards[WHITE_PIECES],to+8);
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
  board.occupancy = board.bitboards[WHITE_PIECES] | board.bitboards[BLACK_PIECES];
}

void unmakeMove(Board &board, Move &m){
  board.flags ^= WHITE_TO_MOVE_BIT;
  int color = (board.flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  board.enPassanTarget = m.getEnPassanTarget();
  if(m.isKingside()){
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    movePiece(board, 1+offset, 3+offset, color+KING, EMPTY);
    movePiece(board, 2+offset, 0+offset, color+ROOK, EMPTY);

    board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board.flags  |= m.getCastlingRights()<<1;
    board.occupancy = board.bitboards[WHITE_PIECES] | board.bitboards[BLACK_PIECES];
    return;
  }
  if(m.isQueenside()){
    int offset= (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    movePiece(board, 5+offset, 3+offset, color+KING, EMPTY);
    movePiece(board, 4+offset, 7+offset, color+ROOK, EMPTY);

    board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board.flags  |= m.getCastlingRights()<<1;
    board.occupancy = board.bitboards[WHITE_PIECES] | board.bitboards[BLACK_PIECES];
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
  setBit(board.bitboards[pieceOnToSquare],from);
  resetBit(board.bitboards[pieceOnToSquare],to);
  board.squares[from] = pieceOnToSquare;

  //undo capture
  byte capturedPiece = m.getCapturedPiece();
  if(capturedPiece != EMPTY)
    setBit(board.bitboards[capturedPiece],to);
  
  board.squares[to] = capturedPiece;
  if(color == BLACK){
    resetBit(board.bitboards[BLACK_PIECES], to);
    setBit(board.bitboards[BLACK_PIECES], from);
    if(capturedPiece != EMPTY) setBit(board.bitboards[WHITE_PIECES], to);
  }else{
    resetBit(board.bitboards[WHITE_PIECES], to);
    setBit(board.bitboards[WHITE_PIECES], from);
    if(capturedPiece != EMPTY) setBit(board.bitboards[BLACK_PIECES], to);
  }
  //undo en passan
  if(m.isEnPassan()){
    if(color == BLACK){
      setBit(board.bitboards[WHITE+PAWN],to+8);
      setBit(board.bitboards[WHITE_PIECES], to+8);
      board.squares[to+8] = WHITE+PAWN;
    }else{
      setBit(board.bitboards[BLACK+PAWN],to-8);
      setBit(board.bitboards[BLACK_PIECES], to-8);
      board.squares[to-8] = BLACK+PAWN;
    }
  }

  //restore board state
  board.flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
  board.flags  |= m.getCastlingRights()<<1;
  board.occupancy = board.bitboards[WHITE_PIECES]|board.bitboards[BLACK_PIECES];
}
