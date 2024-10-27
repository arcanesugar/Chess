#include "magic.h"


Magic rookMagics[64];
Magic bishopMagics[64];
u64 *rookMoves[64];//key, moves bitboard 
u64 *bishopMoves[64];//key, moves bitboard 
u64 rookMasks[64];
u64 bishopMasks[64];

#define ROOK_BLOCKERS_PER_SQUARE 16384
#define BISHOP_BLOCKERS_PER_SQUARE 16384//technically every bishop square has a different amount of blockers, but its fine
//
u64 rookBlockers[64][ROOK_BLOCKERS_PER_SQUARE];
u64 bishopBlockers[64][BISHOP_BLOCKERS_PER_SQUARE];
int numBishopBlockers[64];

static bool quitSearch = false;

void initMagics(){
  generateRookMasks();
  generateBishopMasks();
  if(!loadMagics()){
    fillRookMoves();
    fillBishopMoves();
  }else{
    printf("[error] Could not create sliding move tables\n");
  }
}
void cleanupMagics(){
  printf("[Deallocating rooks]\n");
  //deallocate memory
  for(int i = 0; i<64; i++){
    delete[] rookMoves[i];
  }
  printf("[Deallocating bishops]\n");
  for(int i = 0; i<64; i++){
    delete[] bishopMoves[i];
  }
}

u64 magicHash(Magic magic, u64 blockers){
  return (magic.magic*blockers)>>magic.shift;
}
u64 rookLookup(u64 blockers, byte square){
  blockers = blockers & rookMasks[square]; 
  u64 hashed = magicHash(rookMagics[square],blockers);
  return rookMoves[square][hashed];
}

u64 bishopLookup(u64 blockers, byte square){
  blockers = blockers & bishopMasks[square]; 
  u64 hashed = magicHash(bishopMagics[square], blockers);
  return bishopMoves[square][hashed];
}

//initialisation
void fillRookMoves() {
  int sum = 0;
  for(int i = 0; i<64; i++){
    int size = rookMagics[i].max + 1;
    sum+=size;
    rookMoves[i] = new u64[size];
  }
  printf("%d KiB required for rooks\n", (sum*8)/1000);
  generateRookBlockers();
  for (int i = 0; i < 64; i++) {
    for (int blockerIndex = 0; blockerIndex<ROOK_BLOCKERS_PER_SQUARE; blockerIndex++) {
      u64 blocker = rookBlockers[i][blockerIndex];
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = i / 8;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(&moves, (y * 8) + x);
          if (getBit(blocker, (y * 8) + x))
            break;
          x += directions[direction][0];
          y += directions[direction][1];
        }
      }
      rookMoves[i][magicHash(rookMagics[i], blocker)] = moves;
    }
  }
}

void fillBishopMoves() {
  int sum = 0;
  for(int i = 0; i<64; i++){
    int size = bishopMagics[i].max + 1;
    sum += size;
    bishopMoves[i] = new u64[size];
  }
  printf("%d KiB required for bishops\n", (sum*8)/1000);
  generateBishopBlockers();
  for (int i = 0; i < 64; i++) {
    for (u64 blocker : bishopBlockers[i]) {
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, -1}, {1, 1}, {1, -1}, {-1, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = i / 8;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(&moves, (y * 8) + x);
          if (getBit(blocker, (y * 8) + x))
            break;
          x += directions[direction][0];
          y += directions[direction][1];
        }
      }
      bishopMoves[i][magicHash(bishopMagics[i], blocker)] = moves;
    }
  }
}

//random functions from https://www.chessprogramming.org/Looking_for_Magics
//modified slightly to fit naming convention
u64 random_uint64() {
  u64 u1, u2, u3, u4;
  u1 = (u64)(random()) & 0xFFFF; u2 = (u64)(random()) & 0xFFFF;
  u3 = (u64)(random()) & 0xFFFF; u4 = (u64)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

u64 random_u64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}

void generateRookMasks() {
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      mask = fileMasks[file] | rankMasks[rank];
      resetBit(&mask, (rank * 8) + file);
      rookMasks[(rank * 8) + file] = mask;
    }
  }
}

void generateBishopMasks() {
  int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      for (int i = 0; i < 4; i++) {
        int x = file;
        int y = rank;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(&mask, (y * 8) + x);
          x += directions[i][0];
          y += directions[i][1];
        }
      }
      resetBit(&mask, (rank * 8) + file);
      bishopMasks[(rank * 8) + file] = mask;
    }
  }
}

void generateRookBlockers(){
  for(int i = 0; i<64; i++){
    u64 mask = rookMasks[i];
    u64 blocker = 0;
    for(int j = 0; j<pow(2,bitcount(mask));j++){
      blocker |= ~mask;
      blocker +=1;
      blocker &= mask;
      rookBlockers[i][j] = blocker;  
    }
  }
}
void generateBishopBlockers(){
  for(int i = 0; i<64; i++){
    u64 mask = bishopMasks[i];
    u64 blocker = 0;
    numBishopBlockers[i] = pow(2,bitcount(mask));
    for(int j = 0; j<pow(2,bitcount(mask));j++){
      blocker |= ~mask;
      blocker +=1;
      blocker &= mask;
      bishopBlockers[i][j] = blocker;
    }
  }
}

bool testRookMagic(int square, Magic &magic){
  std::unordered_set<u64> foundKeys;
  u64 max = 0;
  for(int blockerIndex = 0; blockerIndex<ROOK_BLOCKERS_PER_SQUARE; blockerIndex++){
    u64 hashed = magicHash(magic,rookBlockers[square][blockerIndex]);
    max = std::max(hashed,max);
    if(foundKeys.count(hashed) != 0){
      return false;
    }
    foundKeys.insert(hashed);
  }
  magic.max = (int)max;
  return true;
}

bool testBishopMagic(int square, Magic &magic){
  std::unordered_set<u64> foundKeys;//map is probably not the best fit here
  u64 max = 0;
  for(int blockerIndex = 0; blockerIndex<numBishopBlockers[square]; blockerIndex++){
    u64 hashed = magicHash(magic,bishopBlockers[square][blockerIndex]);
    max = std::max(hashed,max);
    if(foundKeys.count(hashed) != 0){
      return false;
    }
    foundKeys.insert(hashed);
  }
  magic.max = (int)max;
  return true;
}

void magicSearch(){
  while(!quitSearch){
    for(int square = 0;square<64;square++){
      if(quitSearch) return;
      Magic magic;
      magic.magic = random_u64_fewbits();
      magic.shift = 61-bitcount(rookMasks[square]); //64-bc would be a perfect magic number, 61 gives wiggle room
      if(testRookMagic(square,magic)){
        if(rookMagics[square].max > magic.max) rookMagics[square] = magic;
      }
      magic.shift = 61-bitcount(bishopMasks[square]);
      if(testBishopMagic(square,magic)){
        if(bishopMagics[square].max > magic.max) bishopMagics[square] = magic;
      }
    }

    //print information about search
    int foundRook = 0;
    int rookTableSize= 0;
    int foundBishop = 0;
    int bishopTableSize= 0;
    for(int i= 0; i<64; i++){
      if(rookMagics[i].max != INT_MAX){
        foundRook++;
        rookTableSize += rookMagics[i].max + 1;
      }
      if(bishopMagics[i].max != INT_MAX){
        foundBishop++;
        bishopTableSize += bishopMagics[i].max + 1;
      }
    }
    printf("\x1b[3A");
    printf("Press enter to quit search\n");
    printf("Rook magics %i/64 %i KiB\n", foundRook, ((rookTableSize*8)/1000));
    printf("Bishop magics %i/64 %i KiB\n", foundBishop, ((bishopTableSize*8)/1000));
  }
}

void searchForMagics(){
  srand(time(NULL));
  printf("[generating blockers]\n");
  generateRookBlockers();
  generateBishopBlockers();
  printf("[beginning search]\nresume from last time?(y/n) ");
  loadMagics();
  char in = getchar();
  if(in == 'n'){
    for(Magic &i : rookMagics){
      i.max = INT_MAX;
    }
    for(Magic &i : bishopMagics){
      i.max = INT_MAX;
    }
  }
  quitSearch = false;
  printf("\n\n\n");//make room for the search output
  std::thread searchThread(magicSearch);
  while(1){
    if(getchar()){
      while(getchar() != '\n');//clear standard input
      break;
    }
  }
  quitSearch = true;
  searchThread.join();
  printf("Save magics?(y/n) \n");
  in = getchar();
  if(in == 'y'){
    printf("saving...\n");
    saveMagics();
  }
  loadMagics();
  while(getchar() != '\n');//clear standard input
};

void saveMagics(){
  std::ofstream file("Search/Magic/magics.txt",std::ofstream::out | std::ofstream::trunc);
  if(!file.is_open()){
    printf("[error] Could not open magics.txt\n");
    return;
  }
  file<<"magicfile2.0\n";
  for(Magic u : rookMagics){
    file<<std::to_string(u.magic)<<"|"<<std::to_string(u.shift)<<"|"<<std::to_string(u.max)<<"\n";
  }
  file<<"Bishop\n";
  for(Magic u : bishopMagics){
    file<<std::to_string(u.magic)<<"|"<<std::to_string(u.shift)<<"|"<<std::to_string(u.max)<<"\n";
  }
};

int loadMagics(){
  std::ifstream file("Search/Magic/magics.txt");
  if(!file.is_open()){
    printf("[error] Could not open magics.txt\n");
    return -1;
  }
  std::string line;
  getline(file,line);
  if(line != "magicfile2.0"){
    printf("Magic file unrecognised, try regenerating with sch\n");
    return -1;
  }
  int index = 0;
  bool rook = true;
  while(getline(file,line)){
    if(line == "Bishop"){
      index = 0;
      rook = false;
      continue;
    }
    std::string tokens[3];
    int tokenIndex = 0;
    for(int i = 0; i<line.length(); i++){
      if(line[i] == '|'){
        tokenIndex ++;
        continue;
      }
      tokens[tokenIndex].push_back(line[i]);
    }
    Magic magic;
    magic.magic = std::stoull(tokens[0]);
    magic.shift = std::stoi(tokens[1]);
    magic.max = std::stoi(tokens[2]);
    if(rook){
      rookMagics[index++] = magic;
    }else{
      bishopMagics[index++] = magic;
    }
  }
  return 0;
};
