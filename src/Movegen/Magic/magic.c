#include "magic.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

#include "../../Core/bitboard.h"
#include "../../Core/board.h"
#include "../../io/io.h"


u64 rookMasks[64];
u64 bishopMasks[64];

static Magic rookMagics[64];
static Magic bishopMagics[64];
static u64 *rookMoves[64];//key, moves bitboard 
static u64 *bishopMoves[64];//key, moves bitboard 

//The rook and bishop blocker tables are only used when creating the moves tables or during a magic search
//They should probobly be moved to local variables, but it works for now

static u64 rookBlockers[64][4096];
static u64 bishopBlockers[64][4096];
static int numBishopBlockers[64];
static int numRookBlockers[64];

static void generateRookBlockers();
static void generateBishopBlockers();

static void generateRookMasks();
static void generateBishopMasks();
static void fillRookMoves();
static void fillBishopMoves();

void initMagics(){
  generateRookMasks();
  generateBishopMasks();
  if(!loadMagics()){
    fillRookMoves();
    fillBishopMoves();
  }else{
    fprintf(stderr,"[error] Could not create sliding move tables\n");
  }
}
void cleanupMagics(){
  //deallocate memory
  for(int i = 0; i<64; i++){
    free(rookMoves[i]);
  }
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
static void fillRookMoves() {
  for(int i = 0; i<64; i++){
    int size = rookMagics[i].max + 1;
    rookMoves[i] = (u64*)malloc(sizeof(u64)*size);
  }
  generateRookBlockers();
  for (int i = 0; i < 64; i++) {
    for (int blockerIndex = 0; blockerIndex<numRookBlockers[i]; blockerIndex++) {
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

static void fillBishopMoves() {
  for(int i = 0; i<64; i++){
    int size = bishopMagics[i].max + 1;
    bishopMoves[i] = (u64*)malloc(sizeof(u64)*size);
  }
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
static u64 random_uint64() {
  u64 u1, u2, u3, u4;
  u1 = (u64)(random()) & 0xFFFF; u2 = (u64)(random()) & 0xFFFF;
  u3 = (u64)(random()) & 0xFFFF; u4 = (u64)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

static u64 random_u64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}

static void generateRookMasks() {
  for (int rank = 0; rank < 8; rank++) {   // y
    for (int file = 0; file < 8; file++) { // x
      u64 mask = (u64)0;
      mask |= fileMasks[file] & 72057594037927680ULL;
      mask |= rankMasks[rank] & 9114861777597660798ULL;
      resetBit(&mask, (rank * 8) + file);
      rookMasks[(rank * 8) + file] = mask;
    }
  }
}

static void generateBishopMasks() {
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
      mask &= 35604928818740736ULL;
      bishopMasks[(rank * 8) + file] = mask;
    }
  }
}

static void generateRookBlockers(){
  for(int i = 0; i<64; i++){
    u64 mask = rookMasks[i];
    u64 blocker = 0;
    numRookBlockers[i] = pow(2,bitcount(mask));
    for(int j = 0; j<pow(2,bitcount(mask));j++){
      blocker |= ~mask;
      blocker +=1;
      blocker &= mask;
      rookBlockers[i][j] = blocker;  
    }
  }
}
static void generateBishopBlockers(){
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

static bool quitSearch = false;
static bool startFromScratch = false;
struct playgroundChunk{
  int counter;
  u64 moves;
};
typedef struct playgroundChunk playgroundChunk;
static playgroundChunk *bishopPlayground;
static playgroundChunk *rookPlayground;
static u64 bishopPlaygroundSize;
static u64 rookPlaygroundSize;
static int searchCounter = 0;

static bool testMagic(Magic *magic, u64 *blockers, int numBlockers, playgroundChunk *playground, int playgroundSize, u64 (*eml)(u64,byte), byte emlc){
  //eml = Exisiting Magic Lookup
  //emlc = eml char argument
  u64 max = 0;
  for(int blockerIndex = 0; blockerIndex < numBlockers; blockerIndex++){
    u64 hashed = magicHash(*magic,blockers[blockerIndex]);
    if(hashed >= playgroundSize) return false;
    if(playground[hashed].counter == searchCounter){
      if(eml == NULL) return false;
      if(playground[hashed].moves != eml(blockers[blockerIndex], emlc)) return false;
    }
    playground[hashed].counter = searchCounter;
    if(eml!=NULL) playground[hashed].moves = eml(blockers[blockerIndex], emlc);
    if(hashed>max) max = hashed;
  }
  magic->max = max;
  return true;
}

static void* magicSearch(void* vargp){
  bishopPlaygroundSize = 4096;
  rookPlaygroundSize = 4096;
  Magic newRookMagics[64];
  Magic newBishopMagics[64];
  u64 (*rookLookupFunction)(u64,byte) = NULL;
  u64 (*bishopLookupFunction)(u64,byte) = NULL;
  for(int i = 0; i<64; i++){
    newRookMagics[i].max = rookPlaygroundSize;
    newBishopMagics[i].max = bishopPlaygroundSize;
  }

  if(!startFromScratch){
    for(int i = 0; i<64; i++){
      newRookMagics[i] = rookMagics[i];
      newBishopMagics[i] = bishopMagics[i];
    }
    rookLookupFunction = rookLookup;
    bishopLookupFunction = bishopLookup;
  }

  //calloc initilises everything to 0 which is important
  rookPlayground   = calloc(rookPlaygroundSize,sizeof(playgroundChunk));
  bishopPlayground = calloc(bishopPlaygroundSize,sizeof(playgroundChunk));
  searchCounter = 1;
  while(!quitSearch){
    for(int square = 0;square<64;square++){
      if(quitSearch) break;
      searchCounter++;
      Magic magic;
      magic.magic = random_u64_fewbits();
      magic.shift = 64-bitcount(rookMasks[square]);
      //using new*Magics[square].max as the playground size means only a better magic will be valid,
      //this does assume that the initial max is within range, which it is
      if(testMagic(
        &magic,
        rookBlockers[square], numRookBlockers[square],
        rookPlayground,newRookMagics[square].max,
        rookLookupFunction,square
      )){
        newRookMagics[square] = magic;
      }

      magic.shift = 64-bitcount(bishopMasks[square]);
      if(testMagic(
        &magic,
        bishopBlockers[square], numBishopBlockers[square],
        bishopPlayground,newBishopMagics[square].max,
        bishopLookupFunction,square
      )){
        newBishopMagics[square] = magic;
      }
    }

    //print information about search
    int foundRook = 0;
    int rookTableSize= 0;
    int foundBishop = 0;
    int bishopTableSize= 0;
    for(int i= 0; i<64; i++){
      if(newRookMagics[i].max != rookPlaygroundSize){
        foundRook++;
        rookTableSize += newRookMagics[i].max + 1;
      }
      if(newBishopMagics[i].max != bishopPlaygroundSize){
        foundBishop++;
        bishopTableSize += newBishopMagics[i].max + 1;
      }
    }
    printf("\x1b[3A");
    printf("Press enter to quit search\n");
    printf("Rook magics %i/64 %i KiB\n", foundRook, ((rookTableSize*8)/1000));
    printf("Bishop magics %i/64 %i KiB\n", foundBishop, ((bishopTableSize*8)/1000));
  }
  for(int i = 0; i<64; i++){
    rookMagics[i] = newRookMagics[i];
    bishopMagics[i] = newBishopMagics[i];
  }
  free(rookPlayground);
  free(bishopPlayground);
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
  FILE *file = fopen("magics.txt","w");
  if(file == NULL){
    printf("[error] Could not open magics.txt\n");
    return;
  }
  fprintf(file,"magicfile2.1\n");
  for(int i = 0; i<64; i++){
    Magic m = rookMagics[i];
    fprintf(file,"%llu %d %d\n",m.magic,m.shift,m.max);
  }
  fprintf(file,"Bishop\n");
  for(int i = 0; i<64; i++){
    Magic m = bishopMagics[i];
    fprintf(file,"%llu %d %d\n",m.magic,m.shift,m.max);
  }
  fclose(file);
};

int loadMagics(){
  FILE *file = fopen("magics.txt", "r");
  if(file == NULL){
    printf("[error] Could not open magics.txt\n");
    return -1;
  }
  rstr line = createRstr();
  rstrFromStream(&line, file);
  if(!rstrEquals(&line,"magicfile2.1")){
    destroyRstr(&line);
    fclose(file);
    printf("Magic file unrecognised, try regenerating with sch\n");
    return -1;
  }
  int index = 0;
  bool rook = true;
  while(!rstrFromStream(&line, file)){
    if(rstrEquals(&line,"Bishop")){
      index = 0;
      rook = false;
      continue;
    }
    char *saveptr;
    Magic magic;
    magic.magic = strtoull(strtok_r(line.buf," ",&saveptr),NULL,10);
    magic.shift = atoi(strtok_r(NULL," ",&saveptr));
    magic.max   = atoi(strtok_r(NULL," ",&saveptr));
    if(rook){
      rookMagics[index++] = magic;
    }else{
      bishopMagics[index++] = magic;
    }
  }
  destroyRstr(&line);
  fclose(file);
  return 0;
};
