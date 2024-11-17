#include "print.h"
#include <stdio.h>
#include <string.h>
#include "../Core/bitboard.h"
#include "../Search/eval.h"


void initPrintSettings(printSettings *settings){
  setUnicodePieces(settings);
  settings->lightColor = 47;
  settings->darkColor = 103;
}

void setASCIIPieces(printSettings *settings){
  char a[14] = "PBNRQKpbnrqk ";
  for(int i = 0; i<13; i++){
    settings->pieceCharacters[i][0] = a[i];
    settings->pieceCharacters[i][1] = '\0';
  }
};

void setUnicodePieces(printSettings *settings){
  char pieces[13][6] = {
    "\u2659", "\u2657", "\u2658", "\u2656", "\u2655", "\u2654",   
    "\u265F", "\u265D", "\u265E", "\u265C", "\u265B", "\u265A", " " 
  };
  for(int i = 0; i<13; i++){
    strcpy(settings->pieceCharacters[i], pieces[i]);
  }
};

void printBitboard(u64 bb){
  printf("\n a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    printf("\x1b[0m%c",'1'+file);
    for(int rank = 7; rank>=0; rank--){
      if(getBit(bb,getSquareIndex(file, rank))) printf("\x1b[45m");
      printf("  \x1b[0m");
    }
    printf("\x1b[0m%c\n",'1'+file);
  }
  printf(" a b c d e f g h\n");
  
}
void printBoard(printSettings settings, Board board, u64 highlighted){
  if(!validateBoard(board)){
    printf("\x1b[31m[error]Board is invalid\x1b[0m\n");
  }
  printf("\n a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    printf("\x1b[0m%c",'1'+file);
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        printf("\x1b[%dm",settings.lightColor);
      }else{
        printf("\x1b[%dm",settings.darkColor);
      }
      if(getBit(highlighted,getSquareIndex(file, rank))) printf("\x1b[45m");
      char *piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      printf("\x1b[30m%s \x1b[0m",piece);
    }
    printf("\x1b[0m%c\n",'1'+file);
  }
  printf(" a b c d e f g h\n");
  if(board.flags&WHITE_TO_MOVE_BIT){
    printf("  White to move\n");
  }else{
    printf("  Black to move\n");
  }
};

void printMoveOnBoard(printSettings settings, Board board, Move m){
  u64 highlightedSquares = (u64)0;
  setBit(&highlightedSquares,getTo(&m));
  setBit(&highlightedSquares,getFrom(&m));
  if(!(board.flags & WHITE_TO_MOVE_BIT)){
    if(isKingside(&m)){
      highlightedSquares = (u64)0;
      setBit(&highlightedSquares,59);
      setBit(&highlightedSquares,57);
    }
    if(isQueenside(&m)){
      highlightedSquares = (u64)0;
      setBit(&highlightedSquares,59);
      setBit(&highlightedSquares,61);
    } 
  }
  else {
    if(isKingside(&m)){
      highlightedSquares = (u64)0;
      setBit(&highlightedSquares,3);
      setBit(&highlightedSquares,1);
    }
    if(isQueenside(&m)){
      highlightedSquares = (u64)0;
      setBit(&highlightedSquares,3);
      setBit(&highlightedSquares,5);
    }
  }
  printBoard(settings,board,highlightedSquares);
};

void printBitboardOnBoard(printSettings settings,Board board,u64 bb){
  printBoard(settings, board, bb);
};

void printPsqt(printSettings settings){
  for(int piece = 6; piece<12; piece++){
    printf("Piece: %s\n", settings.pieceCharacters[piece]);
    for(int file = 7; file>=0; file--){
      for(int rank = 7; rank>=0; rank--){
        double val = pieceSquareTables[piece][getSquareIndex(file,rank)];
        val = val/100;
        if(val>=10){printf("%.1f ",val);continue;}
        printf("%.1f  ",val);
      }
      printf("\n\n\n");
    }
    printf("\n");
  }
  for(int piece = 0; piece<6; piece++){
    printf("Piece: %s\n", settings.pieceCharacters[piece]);
    for(int file = 7; file>=0; file--){
      for(int rank = 7; rank>=0; rank--){
        double val = pieceSquareTables[piece][getSquareIndex(file,rank)];
        val = val/100;
        if(val>=10){printf("%.1f ",val);continue;}
        printf("%.1f  ",val);
      }
      printf("\n\n\n");
    }
    printf("\n");
  }
}
