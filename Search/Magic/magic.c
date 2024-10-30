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
    free(rookMoves[i]);
  }
  printf("[Deallocating bishops]\n");
  for(int i = 0; i<64; i++){
    free(bishopMoves[i]);
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
    rookMoves[i] = (u64*)malloc(sizeof(u64)*size);
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
    bishopMoves[i] = (u64*)malloc(sizeof(u64)*size);
  }
  printf("%d KiB required for bishops\n", (sum*8)/1000);
  generateBishopBlockers();
  for (int i = 0; i < 64; i++) {
    for (int blockerIndex = 0; blockerIndex<numBishopBlockers[i]; blockerIndex++) {
      u64 blocker = bishopBlockers[i][blockerIndex];
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


//Magic generation


//playground is just the first word I thought of

static bool quitSearch = false;
static bool startFromScratch = false;
static u64 *bishopPlayground;
static u64 *rookPlayground;
static u64 bishopPlaygroundSize;
static u64 rookPlaygroundSize;

bool testRookMagic(int square, Magic *magic, u64 bestMax){
  u64 max = 0;
  for(int blockerIndex = 0; blockerIndex<ROOK_BLOCKERS_PER_SQUARE; blockerIndex++){
    u64 hashed = magicHash(*magic,rookBlockers[square][blockerIndex]);
    if(hashed>=rookPlaygroundSize) return false;
    if(hashed>bestMax) return false;
    if(rookPlayground[hashed] == magic->magic) return false;
    rookPlayground[hashed] = magic->magic;
    if(hashed>max) max = hashed;
  }
  magic->max = (int)max;
  return true;
}

bool testBishopMagic(int square, Magic *magic, u64 bestMax){
  u64 max = 0;
  for(int blockerIndex = 0; blockerIndex<numBishopBlockers[square]; blockerIndex++){
    u64 hashed = magicHash(*magic,bishopBlockers[square][blockerIndex]);
    if(hashed>=bishopPlaygroundSize) return false;
    if(hashed>bestMax) return false;
    if(bishopPlayground[hashed] == magic->magic) return false;
    bishopPlayground[hashed] = magic->magic;
    if(hashed>max) max = hashed;
  }
  magic->max = (int)max;
  return true;
}

void* magicSearch(void* vargp){
  bishopPlaygroundSize = 0;
  rookPlaygroundSize = 0;
  if(startFromScratch){
    for(int i = 0; i<64; i++){
      rookMagics[i].max = INT_MAX;
      bishopMagics[i].max = INT_MAX;
    }
    //might be too much room, but it doesnt really matter.
    rookPlaygroundSize = 200000;
    bishopPlaygroundSize = 60000;
  }else{
    for(int i = 0; i<64; i++){
      rookPlaygroundSize += rookMagics[i].max+1;
      bishopPlaygroundSize += bishopMagics[i].max+1;
    }
  }
  rookPlayground   = (u64*)calloc(rookPlaygroundSize,sizeof(u64));
  bishopPlayground = (u64*)calloc(bishopPlaygroundSize,sizeof(u64));
  for(int i = 0; i<99;i++);
  while(!quitSearch){
    for(int square = 0;square<64;square++){
      if(quitSearch) break;
      Magic magic;
      magic.magic = random_u64_fewbits();
      magic.shift = 61-bitcount(rookMasks[square]); //64-bc would be a perfect magic number, 61 gives wiggle room
      if(testRookMagic(square,&magic,rookMagics[square].max)){
        rookMagics[square] = magic;
      }
      magic.shift = 61-bitcount(bishopMasks[square]);
      if(testBishopMagic(square,&magic,bishopMagics[square].max)){
        bishopMagics[square] = magic;
      }
    }

    //print information about search
    int foundRook = 0;
    int rookTableSize= 0;
    int foundBishop = 0;
    int bishopTableSize= 0;
    int maxRook = 0;
    int maxBishop = 0;
    for(int i= 0; i<64; i++){
      if(rookMagics[i].max != INT_MAX){
        foundRook++;
        rookTableSize += rookMagics[i].max + 1;
        if(rookMagics[i].max>maxRook) maxRook = rookMagics[i].max;
      }
      if(bishopMagics[i].max != INT_MAX){
        foundBishop++;
        bishopTableSize += bishopMagics[i].max + 1;
        if(bishopMagics[i].max>maxBishop) maxBishop = bishopMagics[i].max;
      }
    }
    printf("\x1b[3A");
    printf("Press enter to quit search\n");
    printf("Rook magics %i/64 %i KiB\n", foundRook, ((rookTableSize*8)/1000));
    printf("Bishop magics %i/64 %i KiB\n", foundBishop, ((bishopTableSize*8)/1000));
  }
  free(bishopPlayground);
  free(rookPlayground);
  return 0;
}

void searchForMagics(){
  srand(time(NULL));
  printf("[generating blockers]\n");
  generateRookBlockers();
  generateBishopBlockers();
  printf("[beginning search]\nresume from last time?(y/n) ");
  loadMagics();
  char in = getchar();
  startFromScratch = false;
  if(in == 'n')
    startFromScratch = true;
  quitSearch = false;
  printf("\n\n\n");//make room for the search output
  pthread_t thread;//the thread id
  pthread_create(&thread, NULL, magicSearch, NULL);
  while(1){
    if(getchar()){
      while(getchar() != '\n');//clear standard input
      break;
    }
  }
  quitSearch = true;
  pthread_join(thread,NULL);
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
  FILE *file = fopen("Search/Magic/magics.txt","w");
  if(file == NULL){
    printf("[error] Could not open magics.txt\n");
    return;
  }
  fprintf(file,"magicfile2.0\n");
  for(int i = 0; i<64; i++){
    Magic m = rookMagics[i];
    fprintf(file,"%llu|%d|%d\n",m.magic,m.shift,m.max);
  }
  fprintf(file,"Bishop\n");
  for(int i = 0; i<64; i++){
    Magic m = bishopMagics[i];
    fprintf(file,"%llu|%d|%d\n",m.magic,m.shift,m.max);
  }
  fclose(file);
};

int loadMagics(){
  FILE *file = fopen("Search/Magic/magics.txt", "r");
  if(file == NULL){
    printf("[error] Could not open magics.txt\n");
    return -1;
  }
  char line[255];
  if(fgets(line,255,file) == NULL) return -1;
  if(strcmp(line,"magicfile2.0\n") != 0){
    printf("Magic file unrecognised, try regenerating with sch\n");
    return -1;
  }
  int index = 0;
  bool rook = true;
  while(fgets(line,255,file)){
    if(strcmp(line,"Bishop\n") == 0){
      index = 0;
      rook = false;
      continue;
    }
    char tokens[3][100];
    int tokenIndex = 0;
    tokens[0][0] = '\0';
    for(int i = 0; i<strlen(line)-1; i++){
      if(line[i] == '|'){
        tokenIndex ++;
        tokens[tokenIndex][0] = '\0';
        continue;
      }
      strcatchar(tokens[tokenIndex],line[i]);
    }
    char *endptr = NULL;
    Magic magic;
    magic.magic = strtoull(tokens[0],&endptr,10);
    magic.shift = atoi(tokens[1]);
    magic.max = atoi(tokens[2]);
    if(rook){
      rookMagics[index++] = magic;
    }else{
      bishopMagics[index++] = magic;
    }
  }
  fclose(file);
  return 0;
};
