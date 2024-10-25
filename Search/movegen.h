#include "../Board/board.h"
#include "Magic/magic.h"

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

extern u64 knightMoves[64];
extern u64 kingMoves[64];


//initialisation functions
void createKnightTable();//fills the knight moves array, does not do actual move generation
void createKingTable();//see above comment

void addMovesToSquares(MoveList *moves, int fromSquare, u64 squares);
void addMovesFromOffset(MoveList *moves, int offset, u64 targets, byte flags = 0);

void addDiagonalMoves(Board *board, int square, MoveList *moves);
void addOrthogonalMoves(Board *board, int square, MoveList *moves);

void addSlidingMoves(Board *board, MoveList *moves);
void addPawnMoves(Board *board, MoveList *moves);
void addKnightMoves(Board *board, MoveList *moves);
void addKingMoves(Board *board, MoveList *moves);
void addCastlingMoves(Board *board, MoveList *moves);

bool isAttacked(Board *board, byte square, byte opponentColor);
void filterLegalMoves(Board *board, MoveList *moves);

//public:
void initMoveGenerator();
void cleanupMoveGenerator();
void generateMoves(Board *board, MoveList *moves);
