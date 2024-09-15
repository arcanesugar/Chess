#pragma once
#include <fstream>
#include <map>//temporary 
#include <vector>
#include <cmath>
#include <random>
#include "../Board/board.h"
#include "../ui/debug.h"

struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is lust a beter number(and adds room for psedeo legal moves)
  byte end = 0;
  inline void append(Move const &m){moves[end] = m; end++;}
  void remove(byte index){
    for (byte i = index; i < end; i++)
        moves[i] = moves[i + 1]; // copy next element left
    end-=1;
  }
};

class Search{
  
  u64 rookMasks[64];
  u64 bishopMasks[64];
  u64 knightMoves[64];
  u64 kingMoves[64];
  u64 rookMagics[64];
  int rookShifts[64];
  u64 bishopMagics[64];
  int bishopShifts[64];
  bool inFilter = false;
  std::map<u64,u64> rookMoves[64];//key, moves bitboard 
  std::map<u64,u64> bishopMoves[64];//key, moves bitboard 
  
  void generateKnightMoves();//fills the knight moves array, does not do actual move generation
  void generateKingMoves();//see above comment
  void generateFileMasks();
  void generateRankMasks();
  void generateRookMasks();
  void generateBishopMasks();
  void fillRookMoves();
  void fillBishopMoves();

  //Used for magic number search
  std::vector<u64> rookBlockers[64];
  std::vector<u64> bishopBlockers[64];
  bool testMagic(std::vector<u64> *blockers, int square, u64 magic, int shift);
  void generateBlockersFromMask(u64 mask,std::vector<u64> &target);
  void generateRookBlockers();
  void generateBishopBlockers();

  //move generation functions
  u64 friendlyBitboard;
  u64 enemyBitboard;
  int color;//the color of the player whos turn it is
  void addMovesToSquares(MoveList &moves, int fromSquare, u64 squares);
  void addMovesFromOffset(MoveList &moves, int offset, u64 targets, byte flags = 0);
  void addDiagonalMoves(Board board, int square, MoveList &moves);
  void addHorizontalMoves(Board board, int square, MoveList &moves);
  void addPawnMoves(Board board, MoveList &moves);
  void addSlidingMoves(Board board, MoveList &moves);
  void addKnightMoves(Board board, MoveList &moves);
  void addKingMoves(Board board, MoveList &moves);
  void addCastlingMoves(Board board, MoveList &moves);
  void filterLegalMoves(Board board, MoveList &moves);
  //testing
  u64 perftTest(Board &b, int depth, bool root = true);
  
public:
  Search();
  u64 rankMasks[8] = {};
  u64 fileMasks[8] = {};

  void generateMoves(Board board, MoveList &moves);
  void searchForMagics();
  void saveMagics();
  void loadMagics();
  void runMoveGenerationTest(Board &board);
  void runMoveGenerationSuite();
};