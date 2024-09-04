#pragma once
#include "debug.h"
struct MoveList{
  Move moves[255];//maximum number of legal moves possible in a position is 218, 255 is lust a beteer number(and adds room for psedeo legal moves)
  byte end = 0;
  inline void append(Move const &m){moves[end] = m; end++;}
};

class Search{
  u64 hRankMask = (u64)72340172838076673;
  u64 aRankMask = 9259542123273814144U;
  u64 fileMasks[8] = {};
  u64 knightMoves[64];
  void generateKnightMoves();//fills the knight moves array, does not do actual move generation

  void addDiagonalMoves(Board board, int square, MoveList &moves);
  void addHorizontalMoves(Board board, int square, MoveList &moves);
  void addPawnMoves(Board board, MoveList &moves);
  void addSlidingMoves(Board board, MoveList &moves);
  void addKnightMoves(Board board, MoveList &moves);
  void addKingMoves(Board board, MoveList &moves);
  void generateFileMasks();
public:
  void init();
  void generateMoves(Board board, MoveList &moves);

};