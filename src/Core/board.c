#include "board.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../Core/bitboard.h"
#include "../Core/types.h"

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
      setBit(&mask, (y * 8) + i);
    fileMasks[i] = mask;
  }
}

void generateBoardMasks(){
  generateRankMasks();
  generateFileMasks();
}

char getSideToMove(Board *board){
  return (board->flags & WHITE_TO_MOVE_BIT)? WHITE:BLACK;
}
char getOpponentColor(Board *board){
  return (board->flags & WHITE_TO_MOVE_BIT)? BLACK:WHITE;
}
bool validateBoard(Board board) {//Way too expensive to use ouside of debugging
  if(bitScanForward(board.bitboards[WHITE+KING]) == -1) return false;
  if(bitScanForward(board.bitboards[BLACK+KING]) == -1) return false;
  if(board.enPassanTarget == 1) return false;
  int white = 0;
  u64 whitebb = board.bitboards[WHITE+PAWN];
  while(whitebb){
    white++;
    popls1b(&whitebb);
  }
  int black = 0;
  u64 blackbb = board.bitboards[BLACK+PAWN];
  while(blackbb){
    black++;
    popls1b(&blackbb);
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

byte squareNameToIndex(char *squareName, int startIndex) {
  byte squareIndex =
      ((squareName[startIndex+1] - '0' - 1) * 8) + (7 - (squareName[startIndex] - 'a'));
  return squareIndex;
}

Board boardFromFEN(const char *fen){
  Board board;
  char parsed[6][100];
  int index = 0;
  for(int i = 0; i<6; i++){
    parsed[i][0] = '-';
    parsed[i][1] = '\0';
  }
  parsed[0][0] = '\0';
  for(int i = 0; i<strlen(fen); i++){
    if(fen[i] == ' '){
      parsed[++index][0] = '\0';
      continue;
    }
    char temp[2] = "";
    temp[0] = fen[i];
    strcat(parsed[index], temp);
  }
  for(int bbindex = 0; bbindex<14; bbindex++){
    board.bitboards[bbindex] = (u64)0;
  }
  board.occupancy = (u64)0;
  for(int squareIndex = 0; squareIndex<64; squareIndex++){
    board.squares[squareIndex] = EMPTY;
  }
  int squareIndex = 63;
  for(int i = 0; i<strlen(parsed[0]); i++){
    char c = parsed[0][i];
    if(c == '/') continue;
    if(isdigit(c)){
      for(int i = 0; i<c-'0'; i++){
        board.squares[squareIndex] = EMPTY;
        squareIndex -=1;
      }
    }else{
      int color = isupper(c) ? WHITE : BLACK;
      int piece = 0;
      switch(tolower(c)){
        case 'p': piece = PAWN; break;
        case 'b': piece = BISHOP; break;
        case 'n': piece = KNIGHT; break;
        case 'r': piece = ROOK; break;
        case 'k': piece = KING; break;
        case 'q': piece = QUEEN; break;
      }
      setBit(&board.bitboards[color+piece],squareIndex);
      setBit(&board.occupancy, squareIndex);
      if(color == WHITE)
        setBit(&board.bitboards[WHITE_PIECES],squareIndex);
      else
        setBit(&board.bitboards[BLACK_PIECES],squareIndex);
      board.squares[squareIndex] = color+piece;
      squareIndex-=1;
    }
    if(squareIndex<0){break;}
  }
  //set board->flags
  board.flags = (u64)0;
  if(strcmp(parsed[1],"w") == 0)
    board.flags |= WHITE_TO_MOVE_BIT;

  if(strchr(parsed[2], 'K')) board.flags |= WHITE_KINGSIDE_BIT;
  if(strchr(parsed[2], 'Q')) board.flags |= WHITE_QUEENSIDE_BIT;
  if(strchr(parsed[2], 'k')) board.flags |= BLACK_KINGSIDE_BIT;
  if(strchr(parsed[2], 'q')) board.flags |= BLACK_QUEENSIDE_BIT;
  
  board.enPassanTarget = EN_PASSAN_NULL;
  if(parsed[3][0] != '-' && strlen(parsed[3])>1) board.enPassanTarget = squareNameToIndex(parsed[3],0);
  return board;
}

void movePiece(Board *board, byte fromSquare, byte toSquare, byte type, byte capturedPiece){
  board->squares[fromSquare] = EMPTY;
  board->squares[toSquare] = type;

  resetBit(&board->bitboards[type],fromSquare);
  setBit(&board->bitboards[type],toSquare);
  resetBit(&board->bitboards[capturedPiece],toSquare);

  if(type>=BLACK){
    resetBit(&board->bitboards[BLACK_PIECES], fromSquare);
    setBit(&board->bitboards[BLACK_PIECES], toSquare);
    resetBit(&board->bitboards[WHITE_PIECES], toSquare);
  }else{
    resetBit(&board->bitboards[WHITE_PIECES], fromSquare);
    setBit(&board->bitboards[WHITE_PIECES], toSquare);
    resetBit(&board->bitboards[BLACK_PIECES], toSquare);
  }
}

void makeMove(Board *board, Move *m){
  int color = (board->flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  setCastlingRights(m, (board->flags&(0b00011110))>>1);
  setEnPassanTarget(m, board->enPassanTarget);
  board->enPassanTarget = EN_PASSAN_NULL;
  if(isKingside(m)){
    //this side can no longer castle
    if(color == WHITE) board->flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) board->flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (board->flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    movePiece(board, 3+offset, 1+offset, color+KING, EMPTY);
    movePiece(board, 0+offset, 2+offset, color+ROOK, EMPTY);

    board->flags ^= WHITE_TO_MOVE_BIT;
    board->occupancy = board->bitboards[WHITE_PIECES] | board->bitboards[BLACK_PIECES];
    return;
  }
  if(isQueenside(m)){
    //this side can no longer castle
    if(color == WHITE) board->flags &= ~(WHITE_CASTLING_RIGHTS);
    if(color == BLACK) board->flags &= ~(BLACK_CASTLING_RIGHTS);
    
    int offset= (board->flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    movePiece(board, 3+offset, 5+offset, color+KING, EMPTY);
    movePiece(board, 7+offset, 4+offset, color+ROOK, EMPTY);

    board->flags ^= WHITE_TO_MOVE_BIT;
    board->occupancy = board->bitboards[WHITE_PIECES] | board->bitboards[BLACK_PIECES];
    return;
  }

  byte to = getTo(m);
  byte from = getFrom(m);
  if(from < color){
    //std::cout<<"WRONG COLOR\n";
  }
  if(isPromotion(m)){
    resetBit(&board->bitboards[board->squares[from]], from);
    setBit(&board->bitboards[color+getPromotionPiece(m)],from);
    board->squares[from] = color+getPromotionPiece(m);
  }
  byte fromPiece = board->squares[from];
  byte toPiece = board->squares[to];
  movePiece(board, from, to, fromPiece, toPiece);
  setCapturedPiece(m, toPiece);//store captured piece (if there is no piece it will just be empty)
  
  if(fromPiece == color+KING){
    if(board->flags&WHITE_TO_MOVE_BIT){
      board->flags &= ~(WHITE_CASTLING_RIGHTS);
    }else{
      board->flags &= ~(BLACK_CASTLING_RIGHTS);
    }
  }
  
  //update castling rights
  byte rookSquares[4] = {0,7,56,63};
  for(int i = 0; i<4; i++){
    if(from == rookSquares[i] || to == rookSquares[i]){
      board->flags &= ~(WHITE_KINGSIDE_BIT<<i);
    }
  }

  //do en passan
  if(isEnPassan(m)){
    if(board->flags&WHITE_TO_MOVE_BIT){
      resetBit(&board->bitboards[BLACK+PAWN],to-8);
      resetBit(&board->bitboards[BLACK_PIECES],to-8);
      board->squares[to-8] = EMPTY;
    }else{
      resetBit(&board->bitboards[WHITE+PAWN],to+8);
      resetBit(&board->bitboards[WHITE_PIECES],to+8);
      board->squares[to+8] = EMPTY;
    }
  }
  
  //Update en passan target
  if(fromPiece == PAWN || fromPiece == BLACK+PAWN){
    if(abs(to-from)>9){//double forward move
      board->enPassanTarget = to;
      if(board->flags&WHITE_TO_MOVE_BIT){
        board->enPassanTarget -= 8;
      }else{
        board->enPassanTarget +=8;
      }
    }
  }

  board->flags ^= WHITE_TO_MOVE_BIT;
  board->occupancy = board->bitboards[WHITE_PIECES] | board->bitboards[BLACK_PIECES];
}

void unmakeMove(Board *board, Move *m){
  board->flags ^= WHITE_TO_MOVE_BIT;
  int color = (board->flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  board->enPassanTarget = getEnPassanTarget(m);
  if(isKingside(m)){
    int offset= (board->flags & WHITE_TO_MOVE_BIT) ? 0 : 56;
    movePiece(board, 1+offset, 3+offset, color+KING, EMPTY);
    movePiece(board, 2+offset, 0+offset, color+ROOK, EMPTY);

    board->flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board->flags  |= getCastlingRights(m)<<1;
    board->occupancy = board->bitboards[WHITE_PIECES] | board->bitboards[BLACK_PIECES];
    return;
  }
  if(isQueenside(m)){
    int offset= (board->flags & WHITE_TO_MOVE_BIT) ? 0 : 56;

    movePiece(board, 5+offset, 3+offset, color+KING, EMPTY);
    movePiece(board, 4+offset, 7+offset, color+ROOK, EMPTY);

    board->flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
    board->flags  |= getCastlingRights(m)<<1;
    board->occupancy = board->bitboards[WHITE_PIECES] | board->bitboards[BLACK_PIECES];
    return;
  }

  byte to = getTo(m);
  byte from = getFrom(m);

  //if piece was promoted, turn it back to a pawn
  if(isPromotion(m)){
    resetBit(&board->bitboards[board->squares[to]], to);
    setBit(&board->bitboards[PAWN+color],to);
    board->squares[to] = PAWN + color; 
  }
  
  //move piece back
  byte pieceOnToSquare = board->squares[to];
  setBit(&board->bitboards[pieceOnToSquare],from);
  resetBit(&board->bitboards[pieceOnToSquare],to);
  board->squares[from] = pieceOnToSquare;

  //undo capture
  byte capturedPiece = getCapturedPiece(m);
  if(capturedPiece != EMPTY)
    setBit(&board->bitboards[capturedPiece],to);
  
  board->squares[to] = capturedPiece;
  if(color == BLACK){
    resetBit(&board->bitboards[BLACK_PIECES], to);
    setBit(&board->bitboards[BLACK_PIECES], from);
    if(capturedPiece != EMPTY) setBit(&board->bitboards[WHITE_PIECES], to);
  }else{
    resetBit(&board->bitboards[WHITE_PIECES], to);
    setBit(&board->bitboards[WHITE_PIECES], from);
    if(capturedPiece != EMPTY) setBit(&board->bitboards[BLACK_PIECES], to);
  }
  //undo en passan
  if(isEnPassan(m)){
    if(color == BLACK){
      setBit(&board->bitboards[WHITE+PAWN],to+8);
      setBit(&board->bitboards[WHITE_PIECES], to+8);
      board->squares[to+8] = WHITE+PAWN;
    }else{
      setBit(&board->bitboards[BLACK+PAWN],to-8);
      setBit(&board->bitboards[BLACK_PIECES], to-8);
      board->squares[to-8] = BLACK+PAWN;
    }
  }

  //restore board state
  board->flags &= ~(WHITE_CASTLING_RIGHTS  | BLACK_CASTLING_RIGHTS);
  board->flags  |= getCastlingRights(m)<<1;
  board->occupancy = board->bitboards[WHITE_PIECES]|board->bitboards[BLACK_PIECES];
}
