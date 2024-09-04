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
  pawnDestinations = board.bitboards[color + PAWN] &(board.flags&WHITE_TO_MOVE_BIT ? fileMasks[1] : fileMasks[6]);
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
  pawnDestinations &= (board.flags & WHITE_TO_MOVE_BIT) ? ~aRankMask : ~hRankMask;
  pawnDestinations &= opponentBitboard;
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (7 * dir);
    moves.append(move);
  }

  pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
  pawnDestinations &= (board.flags & WHITE_TO_MOVE_BIT) ? ~hRankMask : ~aRankMask;
  pawnDestinations &= opponentBitboard;
  while (pawnDestinations) {
    Move move;
    move.to = popls1b(pawnDestinations);
    move.from = move.to - (9 * dir);
    moves.append(move);
  }
}

void Search::addDiagonalMoves(Board board, int square, MoveList &moves){

};
void Search::addHorizontalMoves(Board board, int square, MoveList &moves){
  
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

void Search::addKingMoves(Board board, MoveList &moves) {}

void Search::init(){
  generateFileMasks();
  generateKnightMoves();
}

void Search::generateFileMasks(){
  for(int i = 0; i<8; i++){
    fileMasks[i] = (u64)255<<(8*i);
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