#include "uci.h"
#include <stdio.h>

#include "../Core/types.h"
#include "../io/rstr.h"
#include "../Core/board.h"
#include "../Search/search.h"
#include "../io/print.h"
#include "../io/tokens.h"
#include "../io/io.h"

void runUCI(){
  rstr input = createRstr();
  TokenList tl;
  createTokenList(&tl);
  bool quit = false;
  Board board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  while(!quit){
    rstrFromStream(&input,stdin);
    tokeniseRstr(&input,&tl);
    if(tl.len == 0) continue;
    if(rstrEqual(&tl.tokens[0],"quit")){
      quit = true;
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"uci")){
      //send name and author of engine
      printf("id name "ENGINE_NAME"\n");
      printf("id author "AUTHOR"\n");

      printf("uciok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"isready")){
      printf("readyok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"position")){
      int movesStart = 0;
      if(rstrEqual(&tl.tokens[1], "fen")){
        board = boardFromFEN(tl.tokens[2].buf);
        if(tl.len == 3) continue;
        if(rstrEqual(&tl.tokens[3], "moves")){
          movesStart = 4;
        }
      }
      if(rstrEqual(&tl.tokens[1], "startpos")){
        board = boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        if(tl.len == 2) continue;
        if(rstrEqual(&tl.tokens[2], "moves")){
          movesStart = 3;
        }
      }
      if(movesStart !=0 && tl.len>movesStart){
        for(int tokenIndex = movesStart; tokenIndex<tl.len; tokenIndex++){
          Move move = moveFromStr(tl.tokens[tokenIndex].buf);
          makeMove(&board,&move);
        }
      }
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"d")){//stockfish also uses d to display the board
      printSettings ps = createDefaultPrintSettings();
      printBoard(ps, board, 0);
    }
    if(rstrEqual(&tl.tokens[0],"echo")){
      if(tl.len<2) {
        printf("not enough arguments");
        continue;
      }
      printf("%s\n", tl.tokens[1].buf);
    }
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}
