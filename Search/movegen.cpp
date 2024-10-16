#include "movegen.h"
MoveGenerator::MoveGenerator(){
  magicman.init();
  generateKnightMoves();
  generateKingMoves();
}
MoveGenerator::~MoveGenerator(){
 magicman.cleanup();
}
void MoveGenerator::generateMoves(Board &board, MoveList &moves) {
  if(!(board.flags&THREATENED_POPULATED)){
    board.flags |= THREATENED_POPULATED;
    generateMoves(board, moves);
    board.flags ^= WHITE_TO_MOVE_BIT;
    generateMoves(board, moves);
    board.flags ^= WHITE_TO_MOVE_BIT;
  }
  friendlyBitboard = (board.flags & WHITE_TO_MOVE_BIT)
     ? board.bitboards[WHITE_PIECES]
     : board.bitboards[BLACK_PIECES];
  enemyBitboard = (board.flags & WHITE_TO_MOVE_BIT)
     ? board.bitboards[BLACK_PIECES]
     : board.bitboards[WHITE_PIECES];

  color = (board.flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  threatenedIndex = (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 1;
  board.threatened[threatenedIndex] = (u64)0;
  moves.end = 0;
  addPawnMoves(board, moves);
  addSlidingMoves(board, moves);
  addKnightMoves(board, moves);
  addKingMoves(board, moves);
  addCastlingMoves(board, moves);
  filterLegalMoves(board, moves);
}

void MoveGenerator::filterLegalMoves(Board board, MoveList &moves){
  byte friendlyColor = color;
  byte opponentColor = (color == WHITE)? BLACK : WHITE;
  for(int i = moves.end-1; i>=0; i--){
    board.makeMove(moves.moves[i]);
    byte kingSquare = bitScanForward(board.bitboards[friendlyColor+KING]);
    bool isLegal = !isAttacked(board, kingSquare, opponentColor);
    board.unmakeMove(moves.moves[i]);
    moves.moves[i].resetUnmakeData();
    if(!isLegal){
       moves.remove(i);
    }
  }
}

bool MoveGenerator::isAttacked(Board const &board, byte square, byte opponentColor){
  //attacked by knight
  u64 possibleKnights = knightMoves[square];
  if(possibleKnights&board.bitboards[KNIGHT+opponentColor]) return true;
  
  //attacked by king
  u64 possibleKings = kingMoves[square];
  if(possibleKings & board.bitboards[KING+opponentColor]) return true;
  //attacked by sliders
  u64 possibleRooks = magicman.rookLookup(board.occupancy, square);
  if(possibleRooks&board.bitboards[ROOK+opponentColor]) return true;

  u64 possibleBishops = magicman.bishopLookup(board.occupancy, square);
  if(possibleBishops&board.bitboards[BISHOP+opponentColor]) return true;

  if((possibleBishops|possibleRooks)&board.bitboards[QUEEN+opponentColor]) return true;
  
  //attacked by pawn
  u64 possiblePawns = u64(0);
  if(opponentColor == WHITE){
    if(square%8 != 0)setBit(possiblePawns, square-9);
    if(square%8 != 7)setBit(possiblePawns, square-7);
  }else{
    if(square%8 != 7)setBit(possiblePawns, square+9);
    if(square%8 != 0)setBit(possiblePawns, square+7);
  }
  if(possiblePawns & board.bitboards[PAWN + opponentColor]) return true;
  
  return false;
}

void MoveGenerator::addSlidingMoves(Board &board, MoveList &moves) {
  u64 horizontalPieces = board.bitboards[color + ROOK] | board.bitboards[color + QUEEN];
  u64 diagonalPieces = board.bitboards[color + BISHOP] | board.bitboards[color + QUEEN];
  while (horizontalPieces) {
    addHorizontalMoves(board, popls1b(horizontalPieces), moves);
  }
  while (diagonalPieces) {
    addDiagonalMoves(board, popls1b(diagonalPieces), moves);
  }
}

void MoveGenerator::addMovesToSquares(MoveList &moves, int fromSquare, u64 squares){
  while (squares) {
    Move move;
    move.setTo(popls1b(squares));
    move.setFrom(fromSquare);
    moves.append(move);
  }
}

void MoveGenerator::addHorizontalMoves(Board &board, int square, MoveList &moves) {
  u64 destinations = magicman.rookLookup(board.occupancy,square) & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void MoveGenerator::addDiagonalMoves(Board &board, int square, MoveList &moves) {
  u64 destinations = magicman.bishopLookup(board.occupancy,square) & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void MoveGenerator::addMovesFromOffset(MoveList &moves, int offset, u64 targets, byte flags){
  while (targets) {
    byte to = popls1b(targets);
    if(to<8 || to>55){ 
      for(int i = BISHOP; i<= QUEEN;i++){
        Move move;
        move.setTo(to);
        move.setFrom(to + offset);
        move.setPromotion(i);
        moves.append(move);
      }
      continue;
    }
    Move move;
    move.setTo(to);
    move.setFrom(to + offset);
    move.setSpecialMoveData(flags);
    moves.append(move);
  }
}

void MoveGenerator::addPawnMoves(Board &board, MoveList &moves) {
  int dir = board.flags & WHITE_TO_MOVE_BIT ? 1 : -1;
  u64 leftFileMask = (board.flags & WHITE_TO_MOVE_BIT) ? Board::fileMasks[7] : Board::fileMasks[0];
  u64 rightFileMask = (board.flags & WHITE_TO_MOVE_BIT) ? Board::fileMasks[0] : Board::fileMasks[7];
  u64 startRank = (board.flags & WHITE_TO_MOVE_BIT ? Board::rankMasks[1] : Board::rankMasks[6]);
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
  board.threatened[threatenedIndex] |= pawnDestinations;
  addMovesFromOffset(moves, -7*dir, pawnDestinations);

  pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
  pawnDestinations &= ~rightFileMask & enemyBitboard;
  board.threatened[threatenedIndex] |= pawnDestinations;
  addMovesFromOffset(moves, -9*dir, pawnDestinations);

  //En Passan
  if(board.enPassanTarget != EN_PASSAN_NULL){
    pawnDestinations = signedShift(board.bitboards[color + PAWN], 7 * dir);
    pawnDestinations &= ~leftFileMask;
    pawnDestinations &= (u64)1<<board.enPassanTarget;
    addMovesFromOffset(moves, -7*dir, pawnDestinations, EN_PASSAN);

    pawnDestinations = signedShift(board.bitboards[color + PAWN], 9 * dir);
    pawnDestinations &= ~rightFileMask;
    pawnDestinations &= (u64)1<<board.enPassanTarget;
    addMovesFromOffset(moves, -9*dir, pawnDestinations,EN_PASSAN);
  }
}

void MoveGenerator::addKnightMoves(Board &board, MoveList &moves) {
  u64 friendlyKnights = board.bitboards[color + KNIGHT];
  while (friendlyKnights) {
    int square = popls1b(friendlyKnights);
    u64 targets = knightMoves[square] & (~friendlyBitboard);
    board.threatened[threatenedIndex] |= targets;
    addMovesToSquares(moves, square, targets);
  }
}

void MoveGenerator::addKingMoves(Board &board, MoveList &moves) {
  int square = bitScanForward(board.bitboards[color + KING]);
  u64 targets = kingMoves[square] & (~friendlyBitboard);
  board.threatened[threatenedIndex] |= targets;
  addMovesToSquares(moves, square, targets);
}

void MoveGenerator::addCastlingMoves(Board &board, MoveList &moves){
  byte opponentColor = (color == WHITE)? BLACK : WHITE;
  if(isAttacked(board,(byte)bitScanForward(board.bitboards[color+KING]),opponentColor)) return;//cannot castle out of check
  int mustBeEmpty[4][3] = {{2,2,1},{4,5,6},{57,58,58},{62,61,60}};
  int mustBeSafe [4][2] = {{2,1},{4,5},{57,58},{61,60}};
  byte masks[4] = {WHITE_KINGSIDE_BIT,WHITE_QUEENSIDE_BIT,BLACK_KINGSIDE_BIT,BLACK_QUEENSIDE_BIT};
  int i = (board.flags&WHITE_TO_MOVE_BIT)? 0 : 2;
  int max = i+2;
  for(int j = i; j<max; j++){
    bool legal = true; 
    for(int s : mustBeEmpty[j]){
      if(board.squares[s] != EMPTY){
        legal = false;
        break;
      }
    }
    if(!legal) continue;
    for(int s : mustBeSafe[j]){
      if(isAttacked(board, s, opponentColor)){
        legal = false;
        break;
      }
    }
    if(!legal) continue;
    if(board.flags&masks[j]){
      Move m;
      if(j%2 == 0){
        m.setSpecialMoveData(CASTLE_KINGSIDE); 
      }else{
        m.setSpecialMoveData(CASTLE_QUEENSIDE); 
      }
      moves.append(m);
    }
  }
}

//Initialisation
void MoveGenerator::generateKnightMoves() {
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

void MoveGenerator::generateKingMoves() {
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
