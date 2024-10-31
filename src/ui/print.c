#include "print.h"
#include <stdio.h>
#include <string.h>
#include "../Core/bitboard.h"
#include "../Search/eval.h"

void setLightColor(printSettings *settings, const char *colorID){
  settings->lightColor[0] = 0;
  strcat(settings->lightColor,"\x1b[");
  strcat(settings->lightColor,colorID);
  strcat(settings->lightColor,"m");
}

void setDarkColor(printSettings *settings, const char *colorID){
  settings->darkColor[0] = 0;
  strcat(settings->darkColor,"\x1b[");
  strcat(settings->darkColor,colorID);
  strcat(settings->darkColor,"m");
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

void printBoard(printSettings settings, Board board, u64 highlighted){
  char str[5000] = "\n";
  if(!validateBoard(board)){
    strcat(str,"\x1b[31m[error]Board is invalid\x1b[0m\n");
  }
  strcat(str," a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    strcat(str, "\x1b[0m");
    strcatchar(str,'1'+file);
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        strcat(str,settings.lightColor);
      }else{
        strcat(str,settings.darkColor);
      }
      if(getBit(highlighted,getSquareIndex(file, rank))) strcat(str,"\x1b[45m");
      char *piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      strcat(str,"\x1b[30m");
      strcat(str,piece);
      strcat(str," \x1b[0m");
    }
    strcat(str, "\x1b[0m");
    strcatchar(str,'1'+file);
    strcat(str,"\n");
  }
  strcat(str," a b c d e f g h\n");
  strcat(str,"");
  printf("%s",str);
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
  for(int piece = 0; piece<6; piece++){
    printf("Piece: %s\n", settings.pieceCharacters[piece]);
    for(int file = 7; file>=0; file--){
      for(int rank = 7; rank>=0; rank--){
        int val = pieceSquareTables[piece][getSquareIndex(file,rank)];
        val = val/10;
        if(val>=100){ printf("%d ",val); continue;}
        if(val>=10){ printf(" %d ",val); continue;}
        printf("  %d ",val);
      }
      printf("\n");
    }
    printf("\n");
  }
}
