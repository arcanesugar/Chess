#include "magic.h"
void MagicMan::init(){
  loadMagics();
  generateRookMasks();
  generateBishopMasks();
  fillRookMoves();
  fillBishopMoves();
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
u64 MagicMan::rookLookup(u64 blockers, byte square){
  blockers = blockers & rookMasks[square]; 
  u64 hashed = (blockers * rookMagics[square]) >> rookShifts[square];
  return rookMoves[square][hashed];
}

u64 MagicMan::bishopLookup(u64 blockers, byte square){
  blockers = blockers & bishopMasks[square]; 
  u64 hashed = (blockers * bishopMagics[square]) >> bishopShifts[square];
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
    rookMovesSizes[i] = pow(2, 64-rookShifts[i]);
    sum += rookMovesSizes[i];
    rookMoves[i] = new u64[rookMovesSizes[i]];
  }
  std::cout<<(sum*8)/1000<<"KiB required for rooks"<<std::endl;
  generateRookBlockers();
  for (int i = 0; i < 64; i++) {
    //allocate memory

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
      rookMoves[i][(blocker * rookMagics[i]) >> rookShifts[i]] = moves;
    }
  }
}

void MagicMan::fillBishopMoves() {
  //see comment in fillRookMoves
  int sum = 0;
  for(int i = 0; i<64; i++){
    bishopMovesSizes[i] = pow(2, 64-bishopShifts[i]);
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
      bishopMoves[i][(blocker * bishopMagics[i]) >> bishopShifts[i]] = moves;
    }
  }
}

//Magic number search

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


void MagicMan::generateRookBlockers(){
  for(int i = 0; i<64; i++){
    generateBlockersFromMask(rookMasks[i], rookBlockers[i]);
  }
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

void MagicMan::generateBishopBlockers(){
  for(int i = 0; i<64; i++){
    generateBlockersFromMask(bishopMasks[i], bishopBlockers[i]);
  }
}

void MagicMan::generateBlockersFromMask(u64 mask,std::vector<u64> &target){
  target.clear();
  u64 bb = mask;
  int bc = bitcount(bb);
  std::vector<int> bitIndices;
  while(bb){
    bitIndices.push_back(popls1b(bb));
  }
  for(int j = 0; j<pow(2,bc);j++){
    u64 blocker = (u64)0;
    for(int f = 0; f<bc; f++){
      blocker |= (((u64)j>>f)&1)<<bitIndices[f];
    }
    target.push_back(blocker);
  }
}

bool MagicMan::testMagic(std::vector<u64> *blockers, int square,u64 magic, int shift){
  std::map<u64,bool> foundKeys;//map is probably not the best fit here
  for(u64 blocker : blockers[square]){
    u64 hashed = u64(blocker*magic)>>shift;
    if(foundKeys.find(hashed) != foundKeys.end()){
      return false;
    }
    foundKeys.insert({hashed,true});
  }
  return true;
}

//Search can be greatly improved
//for example, just using the shift values is an innacurate way to estimate the lookup table size
//and rand is never seeded
void MagicMan::magicSearch(){
  while(!quitSearch){
    for(int square = 0;square<64;square++){
      if(quitSearch) return;
      u64 magic = random_u64_fewbits();
      int shift = -999;
      for(int s = 40; s<64; s++){
        if(testMagic(rookBlockers, square,magic,s)){shift = s;}
        else{break;}
      }
      if(shift >rookShifts[square]){
        rookMagics[square] = magic;
        rookShifts[square] = shift;
      }
      shift = -999;
      for(int s = 40; s<64; s++){
        if(testMagic(bishopBlockers, square,magic,s)){shift = s;}
        else{break;}
      }
      if(shift >bishopShifts[square]){
        bishopMagics[square] = magic;
        bishopShifts[square] = shift;
      }
    }

    //print information about search
    int foundRook = 0;
    int rookTableSize= 0;
    int foundBishop = 0;
    int bishopTableSize= 0;
    for(int i= 0; i<64; i++){
      if(rookShifts[i]!= -999){
        foundRook++;
        rookTableSize += pow(2,64-rookShifts[i]);
      }
      if(bishopShifts[i]!= -999){
        foundBishop++;
        bishopTableSize += pow(2,64-bishopShifts[i]);
      }
    }
    std::cout<<"\x1b[3A";
    std::cout<<"Press enter to quit search"<<std::endl;
    std::cout<<"Rook magics   "<<foundRook  <<"/64 "<<(rookTableSize*8)/1000  <<"KiB "<< rookTableSize   <<" elements"<<std::endl;
    std::cout<<"Bishop magics "<<foundBishop<<"/64 "<<(bishopTableSize*8)/1000<<"KiB "<< bishopTableSize <<" elements"<<std::endl;
  }
}
void MagicMan::searchForMagics(){
  std::cout<<"[generating blockers]\n";
  generateRookBlockers();
  generateBishopBlockers();
  std::cout<<"[beginning search]\nresume from last time?(Y/n)";
  std::string temp;
  std::getline(std::cin, temp);
  loadMagics();
  if(temp == "n"){
    for(int &i : rookShifts){
      i = -999;
    }
    for(int &i : bishopShifts){
      i = -999;
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

void MagicMan::loadMagics(){
  std::ifstream file("Search/Magic/magics.txt");
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
