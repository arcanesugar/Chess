#include "search.h"

#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "../Core/types.h"
#include "../Movegen/movegen.h"
#include "eval.h"
#include "../Core/bitboard.h"

//because -INF must equal N_INF, INT MIN cannot be used
//(-INT_MIN >INT_MAX so it overflows)
#define INF   INT_MAX 
#define N_INF -INF
#define NULL_EVAL 88888

static int nodesSearched = 0;

static void orderMoves(MoveList *ml, int *evaluations){//orders the moves high to low based on the evals list
  for(int i = 0; i<ml->end; i++){
    for(int j = 0; j<i-1; j++){
      if(evaluations[j]<evaluations[j+1]){
        int tempI =  evaluations[j];
        Move tempM =  ml->moves[j];
        evaluations[j] = evaluations[j+1];
        ml->moves[j] = ml->moves[j+1];
        
        evaluations[j+1] = tempI;
        ml->moves[j+1] = tempM;
      }
    }
  }
}
static long long getTimeMS(){//guess what this does? your right! it get the time in miliseconds. Good job
  struct timespec t;
  clock_gettime ( CLOCK_MONOTONIC , & t ) ;
  return t.tv_sec * 1000 + ( t.tv_nsec + 500000 ) / 1000000 ;
}

static int nmax(Board *b, int depth, int alpha, int beta, long long quitTime, bool *quitIfTrue){
  //alpha is the best score we are able to achieve (we being whoevers turn it is)
  //and beta is the best score the opponent is able to achieve
  //why alpha and beta? I dont know, it probably made more sense in original minimax
  //or maybe whoever named them made it intentionally confusing
  if(depth == 0){nodesSearched++; return evaluate(b);}
  MoveList ml = createMoveList();
  generateMoves(b, &ml);
  int bestEval = N_INF;
  for(int i = 0; i<ml.end; i++){
    if(quitTime != 0){
      if(getTimeMS()>quitTime) {return NULL_EVAL;}
    }
    if(quitIfTrue != NULL && *quitIfTrue) return NULL_EVAL;
    makeMove(b,&ml.moves[i]);
    if(inCheck(b,getOpponentColor(b))){unmakeMove(b,&ml.moves[i]); continue;}//opponent will be the side that made the move
    //a move that is good for the opponent is equally bad for us, so we negate all evaluations(including the return)
    //we also swap alpha and beta, because we will be the opponent in the child search
    int eval = -nmax(b,depth-1,-beta, -alpha, quitTime, quitIfTrue);
    unmakeMove(b,&ml.moves[i]);
    if(eval == NULL_EVAL | eval == -NULL_EVAL) return NULL_EVAL;

    if(eval>bestEval){
      bestEval = eval;
    }
    if(bestEval>alpha){
      alpha = bestEval;
    }
    if(beta<=alpha){
      //if the best score the opponent can get is less than the best score we can get, that means 
      //the opponent can force us into a worse score somewere else in the tree,
      //and therefore will never let us get here meaning we can prune the search
      break;
    }
  }
  return bestEval;
}

Move iterativeDeepeningSearch(Board b, int maxDepth, int timeLimit, bool *quitWhenTrue){
  long long startTime = getTimeMS();
  long long quitTime = startTime + (timeLimit - 2);
  if(timeLimit == 0) quitTime = 0;
  MoveList moves = createMoveList();
  generateMoves(&b, &moves);
  byte color = getSideToMove(&b);
  Move bestMove = moves.moves[0];
  int evaluations[255];
  for(int i= 0; i<255; i++) evaluations[i] = N_INF;

  for(int depth = 1; depth <= maxDepth; depth++){
    nodesSearched = 0;
    int bestEval = N_INF;
    orderMoves(&moves, evaluations);
    bool checkmate = true;
    for(int i = 0; i<moves.end;i++){

      makeMove(&b,&moves.moves[i]);
      if(inCheck(&b,color)){unmakeMove(&b,&moves.moves[i]); continue;}
      int eval = -nmax(&b, depth-1,N_INF,-bestEval,quitTime,quitWhenTrue);
      if(eval == NULL_EVAL | eval == -NULL_EVAL) {
        printf("info nodes %d depth %d time %lld\n", nodesSearched, depth, getTimeMS() - startTime);
        return bestMove;
      }
      unmakeMove(&b,&moves.moves[i]);
      checkmate = false;
      evaluations[i] = eval;
      if(eval>bestEval){
        bestEval = eval;
        bestMove = moves.moves[i];
      }
    }
    if(checkmate) return createNullMove();
    printf("info nodes %d depth %d time %lld\n", nodesSearched, depth, getTimeMS() - startTime);
  }

  return bestMove;
};

static u64 perftTest(Board *b, int depth, bool root){
  if(depth <= 0){return 1;}
  u64 count = 0;
  MoveList moves = createMoveList();
  generateMoves(b, &moves);
  byte color = getSideToMove(b);
  for(byte i = 0; i<moves.end;i++){
    makeMove(b,&moves.moves[i]);
    if(inCheck(b,color)){unmakeMove(b,&moves.moves[i]); continue;}
    u64 found = perftTest(b, depth-1,0);
    unmakeMove(b,&moves.moves[i]);
    if(root){
      char movestr[21] = "";
      moveToString(moves.moves[i],movestr);
      printf("%s : %llu\n", movestr, found);
    }
    count += found;
  }
  return count;
}

void runMoveGenerationTest(Board *board){
  for(int i = 1; i<5; i++){
    printf("Depth: %i\n", i);
    u64 found = perftTest(board,i,true);
    printf("Found: %llu\n\n", found);
  }
}

void runMoveGenerationSuite(){
  //https://www.chessprogramming.org/Perft_Results
  char positions[8][100] = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ",
    "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 ",
    "8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - 1 67"
  };
  u64 expected[8] = {
    4865609,
    4085603,
    674624,
    422333,
    2103487,
    3894594,
    3605103,
    279
  };
  int depths[8] = {
    5,
    4,
    5,
    4,
    4,
    4,
    5,
    2
  };
  printf("Starting\n");
  int sum = 0;
  clock_t start = clock();
  Board board;
  for(int i = 0; i<8; i++){
    board = boardFromFEN(positions[i]);
    u64 found = perftTest(&board,depths[i],false);
    sum += found;
    printf("Depth: %i Found: ",depths[i]);
    if(found != expected[i]){
      printf("\x1b[31m");
    }else{
      printf("\x1b[32m");
    }
    printf("%llu/%llu\x1b[0m\n", found, expected[i]);
  }
  clock_t end = clock();
  float durationf = (end-start)/(float)CLOCKS_PER_SEC;
  printf("Searched %i moves\n", sum);
  printf("Finished in %fs\n",durationf);
  printf("%i moves/second", sum/(int)durationf);
}
