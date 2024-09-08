#pragma once
#include <fstream>
#include "../Board/board.h"
#include "../ui/debug.h"

struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is lust a beteer number(and adds room for psedeo legal moves)
  byte end = 0;
  inline void append(Move const &m){moves[end] = m; end++;}
};

class Search{
  

  u64 rookMasks[64];
  u64 bishopMasks[64];
  u64 knightMoves[64];

  u64 rookMagics[64];
  int rookShifts[64];
  u64 bishopMagics[64];
  int bishopShifts[64];

  void generateKnightMoves();//fills the knight moves array, does not do actual move generation
  void generateFileMasks();
  void generateRankMasks();
  void generateRookMasks();
  void generateBishopMasks();
  void generateRookBlockers();
  
  void addDiagonalMoves(Board board, int square, MoveList &moves);
  void addHorizontalMoves(Board board, int square, MoveList &moves);
  void addPawnMoves(Board board, MoveList &moves);
  void addSlidingMoves(Board board, MoveList &moves);
  void addKnightMoves(Board board, MoveList &moves);
  void addKingMoves(Board board, MoveList &moves);
  

public:
u64 rankMasks[8] = {};
u64 fileMasks[8] = {};
  void init();
  void generateMoves(Board board, MoveList &moves);
  void searchForRookMagics();
  void saveMagics();
  void loadMagics();
};