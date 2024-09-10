#include "../search.h"

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

void Search::generateRookBlockers(){
  for(int i = 0; i<64; i++){
    rookBlockers[i].clear();
    u64 bb = rookMasks[i];
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
      rookBlockers[i].push_back(blocker);
    }
  }
}

void Search::generateBishopBlockers(){
  for(int i = 0; i<64; i++){
    bishopBlockers[i].clear();
    u64 bb = bishopMasks[i];
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
      bishopBlockers[i].push_back(blocker);
    }

  }
}

bool Search::testMagic(std::vector<u64> *blockers, int square,u64 magic, int shift){
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
// for example, just using the shift values is an innacurate way to estimate the lookup table size
//and rand is never seeded
void Search::searchForMagics(){
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
  while(1){
    for(int attempt = 0; attempt<100; attempt++){
      for(int square = 0;square<64;square++){
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
        if(rookShifts[i]!= -999)foundRook++;
        rookTableSize += pow(2,64-rookShifts[i]);
        if(bishopShifts[i]!= -999)foundBishop++;
        bishopTableSize += pow(2,64-bishopShifts[i]);
      }
      std::cout<<"\nRook magics found for "<<foundRook<<"/64 squares\n";
      std::cout<<"Rook table ~"<<rookTableSize*8<<" bytes\n";
      std::cout<<"Bishop magics found for "<<foundBishop<<"/64 squares\n";
      std::cout<<"Bishop table ~"<<bishopTableSize*8<<" bytes\n";
    }
    std::string temp;
    std::cout<<"Continue search?(Y/n)\n";
    std::getline(std::cin, temp);
    if(temp == "n"){
      break;
    }
  }
  std::cout<<"Save magics?(y/N)\n";
  std::getline(std::cin, temp);
  if(temp == "y"){
    saveMagics();
  }
  loadMagics();
};

void Search::saveMagics(){
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

void Search::loadMagics(){
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