#include "debug.h"

void setASCIIPieces(printSettings *settings){
  char a[14] = "PBNRQKpbnrqk ";
  int i = 0; 
  for(char p : a){
    settings->pieceCharacters[i++] = p;
  }
};

/*
void setUnicodePieces(printSettings *settings){
  std::string u[13] = {"♙","♗","♘","♖","♕","♔","♟","♝","♞","♜","♛","♚"," "};
  int i = 0; 
  for(std::string p : u){
    settings->pieceCharacters[i++] = p;
  }
};
*/
void printBoard(printSettings settings, Board board, u64 highlighted){
  char str[5000] = "\n";
  if(!validateBoard(board)){
    strcat(str,"\x1b[31m[error]Board is invalid\x1b[0m\n");
  }
  strcat(str," a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    strcat(str, "\x1b[0m");
    char temp1[2] = "n";//n will be replaced 
    temp1[0] = '1'+file;
    strcat(str,temp1);
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        strcat(str,settings.lightColor);
      }else{
        strcat(str,settings.darkColor);
      }
      if(getBit(highlighted,getSquareIndex(file, rank))) strcat(str,"\x1b[45m");
      char piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      strcat(str,"\x1b[30m");
      char temp2[2] = "";
      temp2[0] = piece;
      strcat(str,temp2);
      strcat(str," \x1b[0m");
    }
    strcat(str, "\x1b[0m");
    char temp[3] = "n\n";//n will be replaced 
    temp[0] = '1'+file;
    strcat(str,temp);
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
