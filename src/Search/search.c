#include "search.h"

#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "../Core/types.h"
#include "../Movegen/movegen.h"
#include "eval.h"

//because -INF must equal N_INF, INT MIN cannot be used
//(-INT_MIN >INT_MAX so it overflows)
#define INF   INT_MAX 
#define N_INF -INF

static int nodesSearched = 0;

int nmax(Board *b, int depth, int alpha, int beta){
  //alpha is the best score we are able to achieve (we being whoevers turn it is)
  //and beta is the best score the opponent is able to achieve
  //why alpha and beta? I dont know, it probably made more sense in original minimax
  //or maybe whoever named them made it intentionally confusing
  if(depth == 0){nodesSearched++; return evaluate(b);}
  MoveList ml = createMoveList();
  generateMoves(b, &ml);
  if(ml.end == 0){ nodesSearched++; return N_INF;}//checkmate is the worst possible outcome (for the side whos turn it is)
  int bestEval = N_INF;
  for(int i = 0; i<ml.end; i++){
    makeMove(b,&ml.moves[i]);
    //a move that is good for the opponent is equally bad for us, so we negate all evaluations(including the return)
    //we also swap alpha and beta, because we will be the opponent in the child search
    int eval = -nmax(b,depth-1,-beta, -alpha);
    unmakeMove(b,&ml.moves[i]);
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

Move search(Board b, int depth){
  nodesSearched = 0;
  Move bestMove = createNullMove();
  MoveList ml = createMoveList();
  generateMoves(&b, &ml);
  if(ml.end == 0) return bestMove;//return a nullmove if in checkmate
  
  int bestEval = N_INF;//any move is better than no moves
  //Because we are the root of the tree, there is no beta value and we just use the constant value N_INF.
  //We technically could use a beta and set it to N_INF, but that would be the same as just using the constant.
  //This is a little confusing, and neccisitates this large comment, but I dont care
  for(int i = 0; i<ml.end; i++){
    makeMove(&b,&ml.moves[i]);
    int eval = -nmax(&b,depth-1, N_INF,-bestEval);
    unmakeMove(&b,&ml.moves[i]);
    if(eval>bestEval){
      bestEval = eval;
      bestMove = ml.moves[i];
    }
  }
  printf("Nodes searched: %d\n",nodesSearched);
  return bestMove;
}

u64 perftTest(Board *b, int depth, bool root){
  if(depth <= 0){return 1;}
  u64 count = 0;
  MoveList moves = createMoveList();
  generateMoves(b, &moves);
  for(byte i = 0; i<moves.end;i++){
    makeMove(b,&moves.moves[i]);
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
