#include "search.h"
Search::Search() {
  loadMagics();
  generateFileMasks();
  generateRankMasks();
  generateKnightMoves();
  generateRookMasks();
  generateBishopMasks();
  generateKingMoves();
  fillRookMoves();
  fillBishopMoves();

}
void Search::generateMoves(Board board, MoveList &moves) {
  friendlyBitboard = (board.flags & WHITE_TO_MOVE_BIT)
     ? board.bitboards[WHITE_PIECES]
     : board.bitboards[BLACK_PIECES];
  enemyBitboard = (board.flags & WHITE_TO_MOVE_BIT)
     ? board.bitboards[BLACK_PIECES]
     : board.bitboards[WHITE_PIECES];

  color = (board.flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  moves.end = 0;
  addPawnMoves(board, moves);
  addSlidingMoves(board, moves);
  addKnightMoves(board, moves);
  addKingMoves(board, moves);
  addCastlingMoves(board, moves);
  if(!inFilter){
    filterLegalMoves(board, moves);
  }
}

void Search::filterLegalMoves(Board board, MoveList &moves){
  inFilter = true;
  for(int i = moves.end; i>=0; i--){
    board.makeMove(moves.moves[i]);
    byte kingSquare = popls1b(board.bitboards[color+KING]);
    MoveList responses;
    generateMoves(board, responses);
    bool isLegal = true;
    for(Move response : responses.moves){
      if(response.to == kingSquare){
        isLegal = false;
        break;
      }
    }
    board.unmakeMove(moves.moves[i]);
    if(!isLegal){
       moves.remove(i);
    }
  }
  inFilter = false;
}

void Search::addSlidingMoves(Board board, MoveList &moves) {
  u64 horizontalPieces = board.bitboards[color + ROOK] | board.bitboards[color + QUEEN];
  u64 diagonalPieces = board.bitboards[color + BISHOP] | board.bitboards[color + QUEEN];
  while (horizontalPieces) {
    addHorizontalMoves(board, popls1b(horizontalPieces), moves);
  }
  while (diagonalPieces) {
    addDiagonalMoves(board, popls1b(diagonalPieces), moves);
  }
}

void Search::addMovesFromOffset(MoveList &moves, int offset, u64 targets){
  while (targets) {
    Move move;
    move.to = popls1b(targets);
    move.from = move.to + offset;
    moves.append(move);
  }
}

void Search::addPawnMoves(Board board, MoveList &moves) {
  int dir = board.flags & WHITE_TO_MOVE_BIT ? 1 : -1;
  u64 leftFileMask = (board.flags & WHITE_TO_MOVE_BIT) ? fileMasks[7] : fileMasks[0];
  u64 rightFileMask = (board.flags & WHITE_TO_MOVE_BIT) ? fileMasks[0] : fileMasks[7];
  u64 startRank = (board.flags & WHITE_TO_MOVE_BIT ? rankMasks[1] : rankMasks[6]);
  // forward pawn moves
  u64 pawnDestinations = signedShift(board.bitboards[color + PAWN], 8 * dir);
  pawnDestinations &= ~board.occupancy;
  addMovesFromOffset(moves, -8*dir, pawnDestinations);
  
  // double forward moves
  pawnDestinations = board.bitboards[color + PAWN] & startRank;
  pawnDestinations = signedShift(pawnDestinations, 8 * dir);
  pawnDestinations &=  ~board.occupancy;
  pawnDestinations = signedShift(pawnDestinations, 8 * dir);
  pawnDestinations &=  ~board.occupancy;
  addMovesFromOffset(moves, -16*dir, pawnDestinations);

  // pawn captures
  pawnDestinations = signedShift(board.bitboards[color + PAWN], 7 * dir);
  pawnDestinations &= ~leftFileMask & enemyBitboard;
  addMovesFromOffset(moves, -7*dir, pawnDestinations);

  pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
  pawnDestinations &= ~rightFileMask & enemyBitboard;
  addMovesFromOffset(moves, -9*dir, pawnDestinations);

  //En Passan
  if(board.enPassanTarget != 255){
    pawnDestinations = signedShift(board.bitboards[color + PAWN], 7 * dir);
    pawnDestinations &= ~leftFileMask;
    pawnDestinations &= (u64)1<<board.enPassanTarget;
    addMovesFromOffset(moves, -7*dir, pawnDestinations);

    pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
    pawnDestinations &= ~rightFileMask;
    pawnDestinations &= (u64)1<<board.enPassanTarget;
    addMovesFromOffset(moves, -9*dir, pawnDestinations);
  }
}

void Search::addMovesToSquares(MoveList &moves, int fromSquare, u64 squares){
  while (squares) {
    Move move;
    move.to = popls1b(squares);
    move.from = fromSquare;
    moves.append(move);
  }
}
void Search::addHorizontalMoves(Board board, int square, MoveList &moves) {
  u64 blockers = board.occupancy & rookMasks[square];
  u64 hashed = (blockers * rookMagics[square]) >> rookShifts[square];
  u64 destinations = rookMoves[square][hashed] & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void Search::addDiagonalMoves(Board board, int square, MoveList &moves) {
  u64 blockers = board.occupancy & bishopMasks[square];
  u64 hashed = (blockers * bishopMagics[square]) >> bishopShifts[square];
  u64 destinations = bishopMoves[square][hashed] & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void Search::addKnightMoves(Board board, MoveList &moves) {
  u64 friendlyKnights = board.bitboards[color + KNIGHT];
  while (friendlyKnights) {
    int square = popls1b(friendlyKnights);
    u64 targets = knightMoves[square] & (~friendlyBitboard);
    addMovesToSquares(moves, square, targets);
  }
}

void Search::addKingMoves(Board board, MoveList &moves) {
  int square = popls1b(board.bitboards[color + KING]);
  u64 targets = kingMoves[square] & (~friendlyBitboard);
  addMovesToSquares(moves, square, targets);
}

void Search::addCastlingMoves(Board board, MoveList &moves){
  bool kingside,queenside;
  if(board.flags&WHITE_TO_MOVE_BIT){
    kingside = board.flags&WHITE_KINGSIDE_BIT;
    queenside = board.flags&WHITE_QUEENSIDE_BIT;
    if(kingside){
      if(board.squares[2] == EMPTY && board.squares[1] == EMPTY){
        Move move;
        move.flags |= KINGSIDE_BIT;
        moves.append(move);
      }
    }
    if(queenside){
      if(board.squares[4] == EMPTY && board.squares[5] == EMPTY && board.squares[6] == EMPTY){
        Move move;
        move.flags |= QUEENSIDE_BIT;
        moves.append(move);
      }
    }
  }
  else{
    kingside = board.flags&BLACK_KINGSIDE_BIT;
    queenside = board.flags&BLACK_QUEENSIDE_BIT;
    if(kingside){
      if(board.squares[58] == EMPTY && board.squares[57] == EMPTY){
        Move move;
        move.flags |= KINGSIDE_BIT;
        moves.append(move);
      }
    }
    if(queenside){
      if(board.squares[60] == EMPTY && board.squares[61] == EMPTY && board.squares[62] == EMPTY){
        Move move;
        move.flags |= QUEENSIDE_BIT;
        moves.append(move);
      }
    }
  }
}
// Generate Masks, Move Lookups, ect
//Only need to run once, but should still be understandable


void Search::generateRankMasks() {
  for (int i = 0; i < 8; i++) {
    rankMasks[i] = (u64)255 << (8 * i);
  }
}
void Search::generateFileMasks() {
  for (int i = 0; i < 8; i++) {
    u64 mask = (u64)0;
    for (int y = 0; y < 8; y++) 
      setBit(mask, (y * 8) + i);
    fileMasks[i] = mask;
  }
}

void Search::generateRookMasks() {
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      mask = fileMasks[file] | rankMasks[rank];
      resetBit(mask, (rank * 8) + file);
      rookMasks[(rank * 8) + file] = mask;
    }
  }
}

void Search::generateBishopMasks() {
  int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      for (int i = 0; i < 4; i++) {
        int x = file;
        int y = rank;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(mask, (y * 8) + x);
          x += directions[i][0];
          y += directions[i][1];
        }
      }
      resetBit(mask, (rank * 8) + file);
      bishopMasks[(rank * 8) + file] = mask;
    }
  }
}

void Search::generateKnightMoves() {
  int offsets[8][2] = {{1, -2}, {2, -1}, {2, 1},   {1, 2},
   {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}};
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 moves = (u64)0;
      for (int i = 0; i < 8; i++) {
        int x = file + offsets[i][0];
        int y = rank + offsets[i][1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
          int destination = (y * 8) + x;
          setBit(moves, destination);
        }
      }
      knightMoves[(rank * 8) + file] = moves;
    }
  }
}

void Search::generateKingMoves() {
  int offsets[8][2] = {{1, -1}, {-1, 1}, {-1, -1}, {1, 1},
   {1, 0},  {-1, 0}, {0, -1},  {0, 1}};
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 moves = (u64)0;
      for (int i = 0; i < 8; i++) {
        int x = file + offsets[i][0];
        int y = rank + offsets[i][1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
          int destination = (y * 8) + x;
          setBit(moves, destination);
        }
      }
      kingMoves[(rank * 8) + file] = moves;
    }
  }
}

void Search::fillRookMoves() {
  generateRookBlockers();
  for (int i = 0; i < 64; i++) {
    for (u64 blocker : rookBlockers[i]) {
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = floor(i / 8);
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(moves, (y * 8) + x);
          if (getBit(blocker, (y * 8) + x))
            break;
          x += directions[direction][0];
          y += directions[direction][1];
        }
      }
      rookMoves[i].insert({(blocker * rookMagics[i]) >> rookShifts[i], moves});
    }
  }
}

void Search::fillBishopMoves() {
  generateBishopBlockers();
  for (int i = 0; i < 64; i++) {
    for (u64 blocker : bishopBlockers[i]) {
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, -1}, {1, 1}, {1, -1}, {-1, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = floor(i / 8);
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(moves, (y * 8) + x);
          if (getBit(blocker, (y * 8) + x))
            break;
          x += directions[direction][0];
          y += directions[direction][1];
        }
      }
      bishopMoves[i].insert(
          {(blocker * bishopMagics[i]) >> bishopShifts[i], moves});
    }
  }
}


u64 Search::perftTest(Board &b, int depth, bool root){
  if(depth <= 0){return 1;}
  u64 count = 0;
  MoveList moves;
  generateMoves(b, moves);
  for(byte i = 0; i<moves.end;i++){
    b.makeMove(moves.moves[i]);
    u64 found = perftTest(b, depth-1,false);
    if(root){
      std::string fromStr = "";
      std::string toStr = "";
      fromStr.push_back('a'+(moves.moves[i].from%8));
      toStr.push_back('a'+(moves.moves[i].to%8));
      fromStr.append(std::to_string((moves.moves[i].from/8)+1));
      toStr.append(std::to_string((moves.moves[i].to/8)+1));
      std::cout<<"["<<fromStr<<"->"<<toStr<<"] : ";
      std::cout<<found<<std::endl;
    }
    count += found;
    b.unmakeMove(moves.moves[i]);
  }
  return count;
}

void Search::runMoveGenerationTest(){
  Board board;
  //https://www.chessprogramming.org/Perft_Results
  //position 6
  u64 expected[8] = {
  0,
  20,
  400,
  8902,
  197281,
  4865609,
  119060324,
  3195901860
  };
  board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  for(int i = 1; i<8; i++){
    std::cout<<"\x1b[0mDepth: "<<i<<"\x1b[30m \n";
    u64 found = perftTest(board,i);
    if(found != expected[i]){
      std::cout<<"\x1b[31m";
    }else{
      std::cout<<"\x1b[32m";
    }
    std::cout<<"Found: "<<found<<"/"<<expected[i]<<"\n\n";
  }
}