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
  threatenedIndex = (board.flags & WHITE_TO_MOVE_BIT) ? 0 : 1;
  board.threatened[threatenedIndex] = (u64)0;
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
  byte friendlyColor = color;
  for(int i = moves.end-1; i>=0; i--){
    board.makeMove(moves.moves[i]);
    byte kingSquare = bitScanForward(board.bitboards[friendlyColor+KING]);
    MoveList responses;
    generateMoves(board, responses);
    bool isLegal = true;
    for(int j = 0; j < responses.end; j++){
      if(responses.moves[j].isKingside() || responses.moves[j].isQueenside()){
        continue;
      }
      if(responses.moves[j].getTo() == kingSquare){
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

void Search::addMovesFromOffset(MoveList &moves, int offset, u64 targets, byte flags){
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

void Search::addMovesToSquares(MoveList &moves, int fromSquare, u64 squares){
  while (squares) {
    Move move;
    move.setTo(popls1b(squares));
    move.setFrom(fromSquare);
    moves.append(move);
  }
}
void Search::addHorizontalMoves(Board board, int square, MoveList &moves) {
  u64 blockers = board.occupancy & rookMasks[square];
  u64 hashed = (blockers * rookMagics[square]) >> rookShifts[square];
  u64 destinations = rookMoves[square][hashed] & (~friendlyBitboard);
  board.threatened[threatenedIndex] |= destinations;
  addMovesToSquares(moves, square, destinations);
};

void Search::addDiagonalMoves(Board board, int square, MoveList &moves) {
  u64 blockers = board.occupancy & bishopMasks[square];
  u64 hashed = (blockers * bishopMagics[square]) >> bishopShifts[square];
  u64 destinations = bishopMoves[square][hashed] & (~friendlyBitboard);
  board.threatened[threatenedIndex] |= destinations;
  addMovesToSquares(moves, square, destinations);
};

void Search::addKnightMoves(Board board, MoveList &moves) {
  u64 friendlyKnights = board.bitboards[color + KNIGHT];
  while (friendlyKnights) {
    int square = popls1b(friendlyKnights);
    u64 targets = knightMoves[square] & (~friendlyBitboard);
    board.threatened[threatenedIndex] |= targets;
    addMovesToSquares(moves, square, targets);
  }
}

void Search::addKingMoves(Board board, MoveList &moves) {
  int square = bitScanForward(board.bitboards[color + KING]);
  u64 targets = kingMoves[square] & (~friendlyBitboard);
  board.threatened[threatenedIndex] |= targets;
  addMovesToSquares(moves, square, targets);
}

void Search::addCastlingMoves(Board board, MoveList &moves){
  int pathSquares[4][3] = {{2,2,1},{4,5,6},{57,58,58},{62,61,60}};
  byte masks[4] = {WHITE_KINGSIDE_BIT,WHITE_QUEENSIDE_BIT,BLACK_KINGSIDE_BIT,BLACK_QUEENSIDE_BIT};
  int i = (board.flags&WHITE_TO_MOVE_BIT)? 0 : 2;
  int max = i+2;
  for(int j = i; j<max; j++){
    bool legal = true; 
    for(int s : pathSquares[j]){
      if(board.squares[s] != EMPTY){
        legal = false;
        break;
      }
      if(getBit(board.threatened[!threatenedIndex],s)){
        legal = false;
        break;
      }
    }
    if(!legal) continue;
    Move m;
    if(board.flags&masks[j]){
      if(j%2 == 1){
        m.setSpecialMoveData(CASTLE_KINGSIDE); 
      }else{
        m.setSpecialMoveData(CASTLE_QUEENSIDE); 
      }
      moves.append(m);
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
      if(moves.moves[i].isKingside() || moves.moves[i].isQueenside()){
        fromStr.append("\x1b[33m");
      }
      fromStr.push_back('h'-(moves.moves[i].getFrom()%8));
      toStr.push_back('h'-(moves.moves[i].getTo()%8));
      fromStr.append(std::to_string((moves.moves[i].getFrom()/8)+1));
      toStr.append(std::to_string((moves.moves[i].getTo()/8)+1));
      
      std::cout<<"["<<fromStr<<"->"<<toStr<<"] : ";
      std::cout<<found<<"\x1b[0m"<<std::endl;
    }
    count += found;
    b.unmakeMove(moves.moves[i]);
  }
  return count;
}

void Search::runMoveGenerationTest(Board &board){
  //https://www.chessprogramming.org/Perft_Results
  debug::Settings settings;
  for(int i = 1; i<7; i++){
    std::cout<<"\x1b[0mDepth: "<<i<<"\x1b[30m \n";
    u64 found = perftTest(board,i);
    std::cout<<"\x1b[0mFound: "<<found<<"\n"<<std::endl;
  }
}

void Search::runMoveGenerationSuite(){
  Board board;
  //https://www.chessprogramming.org/Perft_Results
  std::string positions[8] = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ",
    "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 ",
    "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - 1 67"
  };
  u64 expected[8] = {
    4865609,
    4085603,
    674624,
    422333,
    2103487,
    3894594,
    3605103,
    279
  };
  int depths[8] = {
    5,
    4,
    5,
    4,
    4,
    4,
    5,
    2
  };
  std::cout<<"Starting"<<std::endl;
  debug::Settings settings;
  u64 sum = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for(int i = 0; i<8; i++){
    board.loadFromFEN(positions[i]);
    u64 found = perftTest(board,depths[i],false);
    sum += found;
    std::cout<<"Depth: "<<depths[i];
    std::cout<<" Found: ";
    if(found != expected[i]){
      std::cout<<"\x1b[31m";
    }else{
      std::cout<<"\x1b[32m";
    }
    std::cout<<found<<"/"<<expected[i]<<"\x1b[0m"<<std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = end-start;
  std::cout<<"Searched "<< sum << " moves\n";
  std::cout<<"Finished in "<<(float)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()/1000.f<<"s"<<std::endl;
}
