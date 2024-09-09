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
  
};
void Search::addDiagonalMoves(Board board, int square, MoveList &moves){
  
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
  generateRankMasks();
  generateKnightMoves();
  generateRookMasks();
  generateBishopMasks();
  loadMagics();
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

//random functions from https://www.chessprogramming.org/Looking_for_Magics
u64 random_uint64() {
  u64 u1, u2, u3, u4;
  u1 = (u64)(random()) & 0xFFFF; u2 = (u64)(random()) & 0xFFFF;
  u3 = (u64)(random()) & 0xFFFF; u4 = (u64)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

u64 random_u64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}

void Search::searchForMagics(){
  std::cout<<"[generating blockers]\n";
  std::vector<u64> blockers[64];
  for(int i = 0; i<64; i++){
    int bc = bitcount(rookMasks[i]);
    u64 bb = rookMasks[i];
    std::vector<int> bitIndices;
    while(bb){
      bitIndices.push_back(popls1b(bb));
    }
    for(int j = 0; j<pow(2,bc);j++){
      u64 blocker = (u64)0;
      for(int f = 0; f<bc; f++){
        blocker |= (((u64)j>>f)&1)<<bitIndices[f];
      }
      blockers[i].push_back(blocker);
    }
  }
  std::cout<<debug::printBitboard(blockers[14][blockers[14].size()-1]);
  std::cout<<"[beginning search]\n";
  for(int &i : rookShifts){
    i = -999;
  }
  while(1){
    for(int i = 0; i<100; i++){
      for(int j = 0;j<64;j++){
        u64 magic = random_u64_fewbits();
        int shift = -999;
        for(int s = 40; s<64; s++){
          std::map<u64,bool> foundKeys;//map is probably not the best fit here
          bool failure = false;
          for(u64 blocker : blockers[j]){
            u64 hashed = u64(blocker*magic)>>s;
            if(foundKeys.find(hashed) != foundKeys.end()){
              failure = true;
              break;
            }
            foundKeys.insert({hashed,true});
          }
          if(failure) break;
          shift = s;
        }
        if(shift >rookShifts[j]){
          rookMagics[j] = magic;
          rookShifts[j] = shift;
          std::cout<<"Found Magic [square "<<j<<" shift "<<shift<<" magic "<<magic<<"]\n";
        }
      }
      int found = 0;
      int rookTableSize= 0;
      for(int i= 0; i<64; i++){
        if(rookShifts[i]!= -999)found++;
        rookTableSize += pow(2,64-rookShifts[i]);
      }
      std::cout<<"\nMagics found for "<<found<<"/64 squares\n";
      std::cout<<"Rook table ~"<<rookTableSize*8<<" bytes\n\n";
    }
    std::string temp;
    std::cout<<"Continue search?(y/n)\n";
    std::getline(std::cin, temp);
    if(temp == "n"){
      break;
    }
  }
  std::string temp;
  std::cout<<"Save magics?(y/n)\n";
  std::getline(std::cin, temp);
  if(temp == "y"){
    saveMagics();
  }
  loadMagics();
};

void Search::saveMagics(){
  std::ofstream file("Search/magics.txt",std::ofstream::out | std::ofstream::trunc);
  if(!file.is_open()){
    std::cout<<"[error] Could not open magics.txt"<<std::endl;
    return;
  }
  int i = 0;
  for(u64 u : rookMagics){
    file<<std::to_string(u)<<"\n"<<std::to_string(rookShifts[i++])<<"\n";
  }
  file<<"Bishop\n";
  i = 0;
  for(u64 u : bishopMagics){
    file<<std::to_string(u)<<"\n"<<std::to_string(bishopShifts[i++])<<"\n";
  }
};

void Search::loadMagics(){
  std::ifstream file("Search/magics.txt");
  if(!file.is_open()){
    std::cout<<"[error] Could not open magics.txt"<<std::endl;
    return;
  }
  std::string line;
  int index = 0;
  bool rook = true;
  bool shift = false;
  while(getline(file,line)){
    if(line == "Bishop"){
      index = 0;
      shift = false;
      rook = false;
      continue;
    }
    if(shift){
      int val = std::stoi(line);
      if(rook){
        rookShifts[index] = val;
      }else{
        bishopShifts[index] = val;
      }
      index+=1;
      shift = false;
      continue;
    }
    u64 val = std::stoull(line);
    if(rook){
      rookMagics[index] = val;
    }else{
      bishopMagics[index] = val;
    }
    shift = true;
  }
};

