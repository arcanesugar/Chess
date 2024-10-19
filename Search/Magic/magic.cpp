#include "magic.h"
#include <unordered_set>
void MagicMan::init(){
  generateRookMasks();
  generateBishopMasks();
  if(!loadMagics()){
    fillRookMoves();
    fillBishopMoves();
  }else{
    std::cout<<"[error] Could not create sliding move tables"<<std::endl;
  }
}
void MagicMan::cleanup(){
  //deallocate memory
  for(int i = 0; i<64; i++){
    delete[] rookMoves[i];
  }
  for(int i = 0; i<64; i++){
    delete[] bishopMoves[i];
  }
}

u64 MagicMan::magicHash(Magic &magic, u64 blockers){
  return (magic.magic*blockers)>>magic.shift;
}
u64 MagicMan::rookLookup(u64 blockers, byte square){
  blockers = blockers & rookMasks[square]; 
  u64 hashed = magicHash(rookMagics[square],blockers);
  return rookMoves[square][hashed];
}

u64 MagicMan::bishopLookup(u64 blockers, byte square){
  blockers = blockers & bishopMasks[square]; 
  u64 hashed = magicHash(bishopMagics[square], blockers);
  return bishopMoves[square][hashed];
}


//initialisation
void MagicMan::fillRookMoves() {
  //wastes a lot of memory, but it works. Ill fix it later maybe
  //if I could find a fast way of calculating the max used index, I could
  //improve the magic number search, and use less memory. using the highest possible blocker configuation
  //doesnt work becuase the magic number calculation depends on overflow in many cases, so the actuall max result is
  //somewere in the middle. This works for now, ~100MiB is like a chrome tab. Definitely less than ideal though
  int sum = 0;
  for(int i = 0; i<64; i++){
    rookMovesSizes[i] = pow(2, 64-rookMagics[i].shift);
    sum += rookMovesSizes[i];
    rookMoves[i] = new u64[rookMovesSizes[i]];
  }
  std::cout<<(sum*8)/1000<<"KiB required for rooks"<<std::endl;
  generateRookBlockers();
  for (int i = 0; i < 64; i++) {
    for (u64 blocker : rookBlockers[i]) {
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = i / 8;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(moves, (y * 8) + x);
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

void MagicMan::fillBishopMoves() {
  //see comment in fillRookMoves
  int sum = 0;
  for(int i = 0; i<64; i++){
    bishopMovesSizes[i] = pow(2, 64-bishopMagics[i].shift);
    sum += bishopMovesSizes[i];
    bishopMoves[i] = new u64[bishopMovesSizes[i]];
  }
  std::cout<<(sum*8)/1000<<"KiB required for bishops"<<std::endl;
  generateBishopBlockers();
  for (int i = 0; i < 64; i++) {
    for (u64 blocker : bishopBlockers[i]) {
      u64 moves = (u64)0;
      int directions[4][2] = {{-1, -1}, {1, 1}, {1, -1}, {-1, 1}};
      for (int direction = 0; direction < 4; direction++) {
        int x = i % 8;
        int y = i / 8;
        while ((x >= 0 && x < 8) && (y >= 0 && y < 8)) {
          setBit(moves, (y * 8) + x);
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

void MagicMan::generateRookMasks() {
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      mask = Board::fileMasks[file] | Board::rankMasks[rank];
      resetBit(mask, (rank * 8) + file);
      rookMasks[(rank * 8) + file] = mask;
    }
  }
}

void MagicMan::generateBishopMasks() {
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

void MagicMan::generateRookBlockers(){
  for(int i = 0; i<64; i++){
    generateBlockersFromMask(rookMasks[i], rookBlockers[i]);
  }
}
void MagicMan::generateBishopBlockers(){
  for(int i = 0; i<64; i++){
    generateBlockersFromMask(bishopMasks[i], bishopBlockers[i]);
  }
}

void MagicMan::generateBlockersFromMask(u64 mask,std::vector<u64> &target){
  target.clear();
  u64 blocker = 0;
  for(int j = 0; j<pow(2,bitcount(mask));j++){
    blocker |= ~mask;
    blocker +=1;
    blocker &= mask;
    target.push_back(blocker);
  }
}

bool MagicMan::testMagic(std::vector<u64> *blockers, Magic &magic){
  std::unordered_set<u64> foundKeys;//map is probably not the best fit here
  for(u64 blocker : *blockers){
    u64 hashed = magicHash(magic,blocker);
    if(foundKeys.count(hashed) != 0){
      return false;
    }
    foundKeys.insert(hashed);
  }
  return true;
}

void MagicMan::magicSearch(){
  while(!quitSearch){
    for(int square = 0;square<64;square++){
      if(quitSearch) return;
      Magic magic;
      magic.magic = random_u64_fewbits();
      magic.shift = 61-bitcount(rookMasks[square]); //64-bc would be a perfect magic number, 61 gives wiggle room
      if(testMagic(&rookBlockers[square],magic)){
        rookMagics[square] = magic;
      }
      magic.shift = 61-bitcount(bishopMasks[square]);
      if(testMagic(&bishopBlockers[square],magic)){
        bishopMagics[square] = magic;
      }
    }

    //print information about search
    int foundRook = 0;
    int rookTableSize= 0;
    int foundBishop = 0;
    int bishopTableSize= 0;
    for(int i= 0; i<64; i++){
      if(rookMagics[i].shift != -999){
        foundRook++;
        rookTableSize += pow(2,64-rookMagics[i].shift);
      }
      if(bishopMagics[i].shift!= -999){
        foundBishop++;
        bishopTableSize += pow(2,64-bishopMagics[i].shift);
      }
    }
    std::cout<<"\x1b[3A";
    std::cout<<"Press enter to quit search"<<std::endl;
    std::cout<<"Rook magics   "<<foundRook  <<"/64 "<<(rookTableSize*8)/1000  <<"KiB "<< rookTableSize   <<" elements"<<std::endl;
    std::cout<<"Bishop magics "<<foundBishop<<"/64 "<<(bishopTableSize*8)/1000<<"KiB "<< bishopTableSize <<" elements"<<std::endl;
  }
}
void MagicMan::searchForMagics(){
  srand(time(NULL));
  std::cout<<"[generating blockers]\n";
  generateRookBlockers();
  generateBishopBlockers();
  std::cout<<"[beginning search]\nresume from last time?(Y/n)";
  std::string temp;
  std::getline(std::cin, temp);
  loadMagics();
  if(temp == "n"){
    for(Magic &i : rookMagics){
      i.shift = -999;
    }
    for(Magic &i : bishopMagics){
      i.shift = -999;
    }
  }
  quitSearch = false;
  std::thread searchThread(&MagicMan::magicSearch, this);
  std::cin.get();
  quitSearch = true;
  searchThread.join();

  std::cout<<"Save magics?(y/N)\n";
  std::getline(std::cin, temp);
  if(temp == "y"){
    saveMagics();
  }
  loadMagics();
};

void MagicMan::saveMagics(){
  std::ofstream file("Search/Magic/magics.txt",std::ofstream::out | std::ofstream::trunc);
  if(!file.is_open()){
    std::cout<<"[error] Could not open magics.txt"<<std::endl;
    return;
  }
  file<<"magicfile1.0\n";
  for(Magic u : rookMagics){
    file<<std::to_string(u.magic)<<"|"<<std::to_string(u.shift)<<"\n";
  }
  file<<"Bishop\n";
  for(Magic u : bishopMagics){
    file<<std::to_string(u.magic)<<"|"<<std::to_string(u.shift)<<"\n";
  }
};

int MagicMan::loadMagics(){
  std::ifstream file("Search/Magic/magics.txt");
  if(!file.is_open()){
    std::cout<<"[error] Could not open magics.txt"<<std::endl;
    return -1;
  }
  std::string line;
  getline(file,line);
  if(line != "magicfile1.0"){
    std::cout<<"Magic file unrecognised, try regenerating with sch";
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
    std::vector<std::string> tokens;
    int tokenIndex = 0;
    tokens.push_back("");
    for(int i = 0; i<line.length(); i++){
      if(line[i] == '|'){
        tokenIndex ++;
        tokens.push_back("");
        continue;
      }
      tokens[tokenIndex].push_back(line[i]);
    }
    Magic magic;
    magic.magic = std::stoull(tokens[0]);
    magic.shift = std::stoi(tokens[1]);
    magic.max = 0;
    if(rook){
      rookMagics[index++] = magic;
    }else{
      bishopMagics[index++] = magic;
    }
  }
  return 0;
};
