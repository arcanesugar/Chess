#include "search.h"

void Search::generateMoves(Board board, MoveList &moves) {
  moves.end = 0;
  addPawnMoves(board, moves);
  addSlidingMoves(board, moves);
  addKnightMoves(board, moves);
  addKingMoves(board, moves);
}


void Search::addPawnMoves(Board board, MoveList &moves) {
  int dir = board.flags & WHITE_TO_MOVE_BIT ? 1 : -1;
  int color = board.flags & WHITE_TO_MOVE_BIT ? WHITE : BLACK;
  u64 opponentBitboard = (board.flags & WHITE_TO_MOVE_BIT)? board.bitboards[BLACK_PIECES] : board.bitboards[WHITE_PIECES];
  // forward pawn moves
  u64 pawnDestinations = signedShift(board.bitboards[color + PAWN], 8 * dir);
  pawnDestinations &= ~(board.bitboards[WHITE_PIECES]|board.bitboards[BLACK_PIECES]);
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (8 * dir);
    moves.append(move);
  }
  //double forward moves
  pawnDestinations = board.bitboards[color + PAWN] &(board.flags&WHITE_TO_MOVE_BIT ? rankMasks[1] : rankMasks[6]);
  std::cout<<debug::printBitboard(pawnDestinations);
  pawnDestinations = signedShift(pawnDestinations, 16 * dir);
  pawnDestinations &= ~(board.bitboards[WHITE_PIECES]|board.bitboards[BLACK_PIECES]);
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (16 * dir);
    moves.append(move);
  }
  
  // pawn captures
  pawnDestinations = signedShift(board.bitboards[color + PAWN], 7 * dir);
  pawnDestinations &= (board.flags & WHITE_TO_MOVE_BIT) ? ~fileMasks[7] : ~fileMasks[0];
  pawnDestinations &= opponentBitboard;
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (7 * dir);
    moves.append(move);
  }

  pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
  pawnDestinations &= (board.flags & WHITE_TO_MOVE_BIT) ? ~fileMasks[0] : ~fileMasks[7];
  pawnDestinations &= opponentBitboard;
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (9 * dir);
    moves.append(move);
  }
}

void Search::addHorizontalMoves(Board board, int square, MoveList &moves){
  u64 friendlyBitboard = (board.flags & WHITE_TO_MOVE_BIT)? board.bitboards[WHITE_PIECES] : board.bitboards[BLACK_PIECES];
  u64 blockers = board.bitboards[BLACK_PIECES] | board.bitboards[WHITE_PIECES];
  blockers &= rookMasks[square];
  u64 hashed = (blockers*rookMagics[square])>>rookShifts[square];
  u64 destinations = rookMoves[square][hashed]&(~friendlyBitboard);
  resetBit(destinations, square);
  while(destinations){
    Move move;
    move.to = popls1b(destinations);
    move.from = square;
    moves.append(move);
  }
};
void Search::addDiagonalMoves(Board board, int square, MoveList &moves){
  u64 friendlyBitboard = (board.flags & WHITE_TO_MOVE_BIT)? board.bitboards[WHITE_PIECES] : board.bitboards[BLACK_PIECES];
  u64 blockers = board.bitboards[BLACK_PIECES] | board.bitboards[WHITE_PIECES];
  blockers &= bishopMasks[square];
  u64 hashed = (blockers*bishopMagics[square])>>bishopShifts[square];
  u64 destinations = bishopMoves[square][hashed]&(~friendlyBitboard);
  resetBit(destinations, square);
  while(destinations){
    Move move;
    move.to = popls1b(destinations);
    move.from = square;
    moves.append(move);
  }
};

void Search::addSlidingMoves(Board board, MoveList &moves) {
  int color = board.flags & WHITE_TO_MOVE_BIT ? WHITE : BLACK;
  u64 horizontalPieces = board.bitboards[color+ROOK] | board.bitboards[color+QUEEN];
  u64 diagonalPieces = board.bitboards[color+BISHOP] | board.bitboards[color+QUEEN];
  while (horizontalPieces){
    addHorizontalMoves(board, popls1b(horizontalPieces), moves);
  }
  while (diagonalPieces){
    addDiagonalMoves(board, popls1b(diagonalPieces), moves);
  }
}

void Search::addKnightMoves(Board board, MoveList &moves) {
  int color = board.flags & WHITE_TO_MOVE_BIT ? WHITE : BLACK;
  u64 friendlyKnights = board.bitboards[color+KNIGHT];
  u64 friendlyPieces  = (board.flags & WHITE_TO_MOVE_BIT)? board.bitboards[WHITE_PIECES] : board.bitboards[BLACK_PIECES];

  while(friendlyKnights){
    int square = popls1b(friendlyKnights);
    u64 targets = knightMoves[square]&(~friendlyPieces);
    while(targets){
      Move move;
      move.to = popls1b(targets);
      move.from = square;
      moves.append(move);
    }
  }
}

void Search::addKingMoves(Board board, MoveList &moves) {
  int color = board.flags & WHITE_TO_MOVE_BIT ? WHITE : BLACK;
  u64 friendlyPieces  = (board.flags & WHITE_TO_MOVE_BIT)? board.bitboards[WHITE_PIECES] : board.bitboards[BLACK_PIECES];

  int square = popls1b(board.bitboards[color+KING]);
  u64 targets = kingMoves[square]&(~friendlyPieces);
  while(targets){
    Move move;
    move.to = popls1b(targets);
    move.from = square;
    moves.append(move);
  }
}

void Search::init(){
  generateFileMasks();
  generateRankMasks();
  generateKnightMoves();
  generateRookMasks();
  generateBishopMasks();
  generateKingMoves();
  loadMagics();
  fillRookMoves();
  fillBishopMoves();
  std::cout<<debug::printBitboard(bishopBlockers[23][22])<<"\n";
  std::cout<<
  debug::printBitboard(bishopMoves[23][(bishopBlockers[23][22]*bishopMagics[23])>>bishopShifts[23]])<<"\n";
}

void Search::generateRankMasks(){
  for(int i = 0; i<8; i++){
    rankMasks[i] = (u64)255<<(8*i);
  }
}
void Search::generateFileMasks(){
  for(int i = 0; i<8; i++){
    u64 mask = (u64)0;
    for(int y = 0; y<8; y++){
      setBit(mask, (y*8)+i);
    }
    fileMasks[i] = mask;
  }
}

void Search::generateRookMasks(){
  for(int rank = 0; rank<8; rank++){//y
    for(int file = 0; file<8; file++){//x
      u64 mask = (u64)0;
      mask |= fileMasks[file];
      mask |= rankMasks[rank];
      resetBit(mask,(rank*8)+file);
      rookMasks[(rank*8)+file] = mask;
    }
  }
}

void Search::generateBishopMasks(){
  for(int rank = 0; rank<8; rank++){//y
    for(int file = 0; file<8; file++){//x
      u64 mask = (u64)0;
      int directions [4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
      for(int i = 0; i<4; i++){
        int x = file;
        int y = rank;
        while((x>=0 && x<8)&&(y>=0&&y<8)){
          setBit(mask,(y*8)+x); 
          x+=directions[i][0];
          y+=directions[i][1];
        }
      }
      resetBit(mask,(rank*8)+file);
      bishopMasks[(rank*8)+file] = mask;
    }
  }
}

void Search::generateKnightMoves(){
  for(int rank = 0; rank<8; rank++){//y
    for(int file = 0; file<8; file++){//x  
      u64 moves = (u64)0;
      int offsets[8][2] = {
          {1,-2},
          {2,-1},
          {2,1},
          {1,2},
          {-1,2},
          {-2,1},
          {-2,-1},
          {-1,-2}
      };
      for(int i = 0; i<8; i++){
        int x = file +offsets[i][0];
        int y = rank +offsets[i][1];
        if(x>=0 && x<8 && y>=0 && y<8){
          int destination = (y*8)+x;
          setBit(moves,destination);
        }
      }
      knightMoves[(rank*8)+file] = moves;
    }
  }
}

void Search::generateKingMoves(){
  for(int rank = 0; rank<8; rank++){//y
    for(int file = 0; file<8; file++){//x  
      u64 moves = (u64)0;
      int offsets[8][2] = {
      {1,-1},
      {-1,1},
      {-1,-1},
      {1,1},
      {1,0},
      {-1,0},
      {0,-1},
      {0,1}
      };
      for(int i = 0; i<8; i++){
        int x = file +offsets[i][0];
        int y = rank +offsets[i][1];
        if(x>=0 && x<8 && y>=0 && y<8){
          int destination = (y*8)+x;
          setBit(moves,destination);
        }
      }
      kingMoves[(rank*8)+file] = moves;
    }
  }
}

void Search::fillRookMoves(){
  generateRookBlockers();
  for(int i = 0; i<64; i++){
    for(u64 blocker : rookBlockers[i]){
      u64 moves = (u64)0;
      int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
      for(int direction = 0; direction<4; direction++){
        int x = i%8;
        int y = floor(i/8);
        while((x>=0 && x<8)&&(y>=0 && y<8)){
          setBit(moves,(y*8)+x);
          if(getBit(blocker,(y*8)+x))break;
          x+=directions[direction][0];
          y+=directions[direction][1];
        }
      }
      rookMoves[i].insert({(blocker*rookMagics[i])>>rookShifts[i],moves});
    }
  }
}

void Search::fillBishopMoves(){
  generateBishopBlockers();
  for(int i = 0; i<64; i++){
    for(u64 blocker : bishopBlockers[i]){
      u64 moves = (u64)0;
      int directions[4][2] = {{-1,-1},{1,1},{1,-1},{-1,1}};
      for(int direction = 0; direction<4; direction++){
        int x = i%8;
        int y = floor(i/8);
        while((x>=0 && x<8)&&(y>=0 && y<8)){
          setBit(moves,(y*8)+x);
          if(getBit(blocker,(y*8)+x))break;
          x+=directions[direction][0];
          y+=directions[direction][1];
        }
      }
      bishopMoves[i].insert({(blocker*bishopMagics[i])>>bishopShifts[i],moves});
    }
  }
}

