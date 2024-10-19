#include <iostream>
#include <fstream>
#include <vector> 
#include <map> 
#include <thread>
#include "../../Board/board.h"

struct Magic{
  u64 magic;
  int shift;
  int max;
};
class MagicMan{//magic manager, or at least thats my justification
  //the actual magic numbers
  Magic rookMagics[64];
  Magic bishopMagics[64];

  //used for generating magic numbers 
  bool quitSearch = false;
  std::vector<u64> rookBlockers[64];
  std::vector<u64> bishopBlockers[64];

  bool testMagic(std::vector<u64> *blockers, int square, u64 magic, int shift);
  void magicSearch();
  void generateBlockersFromMask(u64 mask,std::vector<u64> &target);
  void generateRookBlockers();
  void generateBishopBlockers();

  //using magic numbers
  void generateRookMasks();
  void generateBishopMasks();
  void fillRookMoves();
  void fillBishopMoves();
  u64 *rookMoves[64];//key, moves bitboard 
  u64 *bishopMoves[64];//key, moves bitboard 
  int rookMovesSizes[64];
  int bishopMovesSizes[64];
  u64 rookMasks[64];
  u64 bishopMasks[64];
  
public:
  //using init and cleanup instead of a constructor/deconstructor
  //lets you do things like run a magic search without generating rook/bishop move lookups
  void init();  
  void cleanup();   
  void searchForMagics();
  void saveMagics();
  void loadMagics();

  u64 rookLookup(u64 blockers, byte rookSquare);
  u64 bishopLookup(u64 blockers, byte bishopSquare);
};
