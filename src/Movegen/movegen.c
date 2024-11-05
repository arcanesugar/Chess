#include "movegen.h"

#include "Magic/magic.h"
#include "../Core/bitboard.h"

u64 knightMoves[64];
u64 kingMoves[64];

//initialisation functions
void createKnightTable();//fills the knight moves array, does not do actual move generation
void createKingTable();//see above comment

void addMovesToSquares(MoveList *moves, int fromSquare, u64 squares);
void addMovesFromOffset(MoveList *moves, int offset, u64 targets, byte flags);

void addDiagonalMoves(Board *board, int square, MoveList *moves);
void addOrthogonalMoves(Board *board, int square, MoveList *moves);

void addSlidingMoves(Board *board, MoveList *moves);
void addPawnMoves(Board *board, MoveList *moves);
void addKnightMoves(Board *board, MoveList *moves);
void addKingMoves(Board *board, MoveList *moves);
void addCastlingMoves(Board *board, MoveList *moves);

void filterLegalMoves(Board *board, MoveList *moves);

void initMoveGenerator(){
  generateBoardMasks();
  initMagics();
  createKingTable();
  createKnightTable();
}

void cleanupMoveGenerator(){
  cleanupMagics();
}


u64 friendlyBitboard;
u64 enemyBitboard;
byte color;
byte opponentColor;

void generateMoves(Board *board, MoveList *moves) {
  friendlyBitboard = (board->flags & WHITE_TO_MOVE_BIT)
     ? board->bitboards[WHITE_PIECES]
     : board->bitboards[BLACK_PIECES];
  enemyBitboard = (board->flags & WHITE_TO_MOVE_BIT)
     ? board->bitboards[BLACK_PIECES]
     : board->bitboards[WHITE_PIECES];
  color = (board->flags & WHITE_TO_MOVE_BIT) ? WHITE : BLACK;
  opponentColor = (color == WHITE)? BLACK : WHITE;
  moves->end = 0;

  addPawnMoves(board, moves);
  addSlidingMoves(board, moves);
  addKnightMoves(board, moves);
  addKingMoves(board, moves);
  addCastlingMoves(board, moves);
  filterLegalMoves(board, moves);
}

void filterLegalMoves(Board *board, MoveList *moves){
  for(int i = moves->end-1; i>=0; i--){
    makeMove(board,&moves->moves[i]);
    byte kingSquare = bitScanForward(board->bitboards[color+KING]);
    bool isLegal = !isAttacked(board, kingSquare, opponentColor);
    unmakeMove(board,&moves->moves[i]);
    moves->moves[i].unmakeData = 0;
    if(!isLegal){
      moveListRemove(moves,i);
    }
  }
}

bool isAttacked(Board *board, byte square, byte opponentColor){
  //attacked by knight
  u64 possibleKnights = knightMoves[square];
  if(possibleKnights&board->bitboards[KNIGHT+opponentColor]) return true;

  //attacked by king
  u64 possibleKings = kingMoves[square];
  if(possibleKings & board->bitboards[KING+opponentColor]) return true;

  //attacked by sliders
  u64 possibleRooks = rookLookup(board->occupancy, square);
  if(possibleRooks&board->bitboards[ROOK+opponentColor]) return true;

  u64 possibleBishops = bishopLookup(board->occupancy, square);
  if(possibleBishops&board->bitboards[BISHOP+opponentColor]) return true;

  if((possibleBishops|possibleRooks)&board->bitboards[QUEEN+opponentColor]) return true;
  
  //attacked by pawn
  u64 possiblePawns = 0;
  if(opponentColor == WHITE){
    if(square%8 != 0)setBit(&possiblePawns, square-9);
    if(square%8 != 7)setBit(&possiblePawns, square-7);
  }else{
    if(square%8 != 7)setBit(&possiblePawns, square+9);
    if(square%8 != 0)setBit(&possiblePawns, square+7);
  }
  if(possiblePawns & board->bitboards[PAWN + opponentColor]) return true;
  
  return false;
}

void addSlidingMoves(Board *board, MoveList *moves) {
  u64 orthogonalPieces = board->bitboards[color + ROOK] | board->bitboards[color + QUEEN];
  u64 diagonalPieces = board->bitboards[color + BISHOP] | board->bitboards[color + QUEEN];
  while (orthogonalPieces) {
    addOrthogonalMoves(board, popls1b(&orthogonalPieces), moves);
  }
  while (diagonalPieces) {
    addDiagonalMoves(board, popls1b(&diagonalPieces), moves);
  }
}

void addOrthogonalMoves(Board *board, int square, MoveList *moves) {
  u64 destinations = rookLookup(board->occupancy,square) & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void addDiagonalMoves(Board *board, int square, MoveList *moves) {
  u64 destinations = bishopLookup(board->occupancy,square) & (~friendlyBitboard);
  addMovesToSquares(moves, square, destinations);
};

void addMovesFromOffset(MoveList *moves, int offset, u64 targets, byte flags){
  while (targets) {
    byte to = popls1b(&targets);
    if(to<8 || to>55){ 
      for(int i = BISHOP; i<= QUEEN;i++){
        Move move = createEmptyMove();
        setTo(&move, to);
        setFrom(&move, to + offset);
        setPromotion(&move, i);
        moveListAppend(moves,move);
      }
      continue;
    }
    Move move = createEmptyMove();
    setTo(&move, to);
    setFrom(&move, to + offset);
    setSpecialMoveData(&move, flags);
    moveListAppend(moves,move);
  }
}

void addPawnMoves(Board *board, MoveList *moves) {
  int dir = board->flags & WHITE_TO_MOVE_BIT ? 1 : -1;
  u64 leftFileMask = (board->flags & WHITE_TO_MOVE_BIT) ? fileMasks[7] : fileMasks[0];
  u64 rightFileMask = (board->flags & WHITE_TO_MOVE_BIT) ? fileMasks[0] : fileMasks[7];
  u64 startRank = (board->flags & WHITE_TO_MOVE_BIT ? rankMasks[1] : rankMasks[6]);
  // forward pawn moves
  u64 pawnDestinations = signedShift(board->bitboards[color + PAWN], 8 * dir);
  pawnDestinations &= ~board->occupancy;
  addMovesFromOffset(moves, -8*dir, pawnDestinations,0);
  
  // double forward moves
  pawnDestinations = board->bitboards[color + PAWN] & startRank;
  pawnDestinations = signedShift(pawnDestinations, 8 * dir);
  pawnDestinations &=  ~board->occupancy;
  pawnDestinations = signedShift(pawnDestinations, 8 * dir);
  pawnDestinations &=  ~board->occupancy;
  addMovesFromOffset(moves, -16*dir, pawnDestinations,0);

  // pawn captures
  pawnDestinations = signedShift(board->bitboards[color + PAWN], 7 * dir);
  pawnDestinations &= ~leftFileMask & enemyBitboard;
  addMovesFromOffset(moves, -7*dir, pawnDestinations,0);

  pawnDestinations = signedShift(board->bitboards[color + PAWN], 9 * dir);
  pawnDestinations &= ~rightFileMask & enemyBitboard;
  addMovesFromOffset(moves, -9*dir, pawnDestinations,0);

  //En Passan
  if(board->enPassanTarget != EN_PASSAN_NULL){
    pawnDestinations = signedShift(board->bitboards[color + PAWN], 7 * dir);
    pawnDestinations &= ~leftFileMask;
    pawnDestinations &= (u64)1<<board->enPassanTarget;
    addMovesFromOffset(moves, -7*dir, pawnDestinations, EN_PASSAN);

    pawnDestinations = signedShift(board->bitboards[color + PAWN], 9 * dir);
    pawnDestinations &= ~rightFileMask;
    pawnDestinations &= (u64)1<<board->enPassanTarget;
    addMovesFromOffset(moves, -9*dir, pawnDestinations,EN_PASSAN);
  }
}

void addMovesToSquares(MoveList *moves, int fromSquare, u64 squares){
  while (squares) {
    Move move = createEmptyMove();
    setTo(&move, popls1b(&squares));
    setFrom(&move, fromSquare);
    moveListAppend(moves,move);
  }
}

void addKnightMoves(Board *board, MoveList *moves) {
  u64 friendlyKnights = board->bitboards[color + KNIGHT];
  while (friendlyKnights) {
    int square = popls1b(&friendlyKnights);
    u64 targets = knightMoves[square] & (~friendlyBitboard);
    addMovesToSquares(moves, square, targets);
  }
}
void addKingMoves(Board *board, MoveList *moves) {
  int square = bitScanForward(board->bitboards[color + KING]);
  u64 targets = kingMoves[square] & (~friendlyBitboard);
  addMovesToSquares(moves, square, targets);
}

void addCastlingMoves(Board *board, MoveList *moves){
  if(isAttacked(board,(byte)bitScanForward(board->bitboards[color+KING]),opponentColor)) return;//cannot castle out of check
  u64 mustBeEmpty[4] = {6ULL,112ULL,432345564227567616ULL,8070450532247928832ULL};
  byte mustBeSafe [4][2] = {{2,1},{4,5},{57,58},{61,60}};
  byte masks[4] = {WHITE_KINGSIDE_BIT,WHITE_QUEENSIDE_BIT,BLACK_KINGSIDE_BIT,BLACK_QUEENSIDE_BIT};
  int i = (board->flags&WHITE_TO_MOVE_BIT)? 0 : 2;
  int max = i+2;
  for(int j = i; j<max; j++){
    if(!(board->flags&masks[j])) continue;
    if(mustBeEmpty[j]&board->occupancy) continue;
    bool legal = true; 
    for(int s = 0; s<2; s++){
      if(isAttacked(board, mustBeSafe[j][s], opponentColor)){
        legal = false;
        break;
      }
    }
    if(!legal) continue;
    Move m = createEmptyMove();
    if(j%2 == 0){
      setSpecialMoveData(&m,CASTLE_KINGSIDE); 
    }else{
      setSpecialMoveData(&m,CASTLE_QUEENSIDE); 
    }
    moveListAppend(moves,m);
  }
}

//Initialisation
void createKnightTable() {
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
          setBit(&moves, destination);
        }
      }
      knightMoves[(rank * 8) + file] = moves;
    }
  }
}

void createKingTable() {
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
          setBit(&moves, destination);
        }
      }
      kingMoves[(rank * 8) + file] = moves;
    }
  }
}
