#include "debug.h"
void debug::Settings::setASCIIPieces(){
  std::string a[13] = {"P","B","N","R","Q","K","p","b","n","r","q","k"," "};
  int i = 0; 
  for(std::string p : a){
    pieceCharacters[i++] = p;
  }
};
void debug::Settings::setUnicodePieces(){
  std::string u[13] = {"♙","♗","♘","♖","♕","♔","\x1b[1m♟","♝","♞","♜","♛","♚"," "};
  int i = 0; 
  for(std::string p : u){
    pieceCharacters[i++] = p;
  }
};

std::string debug::testFormatting(bool highlightOnly){
  std::string str = "";
  std::string prefix[4] = {"3","4","9","10"};
  for(int p = 0; p<4; p++){
    if(highlightOnly && p%2 == 0) continue;
    for(int i = 0; i<8; i++){
      std::string num = prefix[p] + std::to_string(i);
      str.append("\x1b[" + num + "m" + num);
    }
    str.append("\x1b[0m\n");
  }
  return str;
};

std::string debug::printBoard(Settings settings, Board const &board){
  std::string str = "";
  str.append(" a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    str.append(std::to_string(file+1));
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        str.append(settings.lightColor);
      }else{
        str.append(settings.darkColor);
      }
      std::string piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      str.append(piece + " \x1b[0m");
    }
    str.append("\x1b[0m" + std::to_string(file+1)+ "\n");
  }
  str.append(" a b c d e f g h\n");
  return str;
};

std::string debug::printMove(Settings settings, Board const &board, Move &m){
  std::string str = "";
  str.append(" a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    str.append(std::to_string(file+1));
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        str.append(settings.lightColor);
      }else{
        str.append(settings.darkColor);
      }
      if(m.from == getSquareIndex(file,rank) || m.to == getSquareIndex(file,rank)){
        str.append("\x1b[45m");
      }
      std::string piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      str.append(piece + " \x1b[0m");
    }
    str.append("\x1b[0m" + std::to_string(file+1)+ "\n");
  }
  str.append(" a b c d e f g h\n");
  return str;
};

std::string debug::printBitboard(u64 const &bb){
  std::string str = "";
  for(int file = 7; file>=0; file--){
    for(int rank = 7; rank>=0; rank--){
      if(getBit(bb,getSquareIndex(file,rank))){
        str.append("\x1b[42m");
      }else{
        str.append("\x1b[41m");
      }
      str.append("  ");
    }
    str.append("\x1b[0m\n");
  }
  return str;
};