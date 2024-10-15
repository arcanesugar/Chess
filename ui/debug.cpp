#include "debug.h"
void debug::Settings::setASCIIPieces(){
  std::string a[13] = {"P","B","N","R","Q","K","p","b","n","r","q","k"," "};
  int i = 0; 
  for(std::string p : a){
    pieceCharacters[i++] = p;
  }
};
void debug::Settings::setUnicodePieces(){
  std::string u[13] = {"♙","♗","♘","♖","♕","♔","♟","♝","♞","♜","♛","♚"," "};
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
      if(num.size()== 2){
        str.append("\x1b[0m  " + num + "\x1b[" + num + "m  " );
        continue;
      }
      str.append("\x1b[0m " + num + "\x1b[" + num + "m  " );
    }
    str.append("\x1b[0m\n");
  }
  return str;
};

std::string debug::moveToStr(Move m, bool expanded){
  std::string str = "";
  if(expanded){
    
    str.append("Move Data:   ");
    for(int i = 15; i>=0;i--){
      str.append("\x1b[34m");
      if(i>=4) str.append("\x1b[33m");
      if(i>=10) str.append("\x1b[32m");
      if((m.getMoveData()>>i)&(unsigned short)1){
        str.push_back('1');
      }else{
        str.push_back('0');
      }
    }
    str.append("\x1b[0m");
    str.append("\nUnmake Data: ");
    for(int i = 15; i>=0;i--){
      str.append("\x1b[2;30m");
      if(i>=2) str.append("\x1b[0;34m");
      if(i>=6) str.append("\x1b[0;33m");
      if(i>=12) str.append("\x1b[0;32m");
      if((m.getUnmakeData()>>i)&(unsigned short)1){
        str.push_back('1');
      }else{
        str.push_back('0');
      }
    }
    str.append("\n\x1b[0m");
    str.append("isKingside: " + std::to_string(m.isKingside())+"\n");
    str.append("isQueenside: " + std::to_string(m.isQueenside())+"\n");
    str.append("isEnPassan: " + std::to_string(m.isEnPassan())+"\n");
    str.append("isPromotion: " + std::to_string(m.isPromotion())+"\n");
    str.append("toSquare: " + std::to_string(m.getTo())+"\n");
    str.append("fromSquare: " + std::to_string(m.getFrom())+"\n");
    str.append("promotionPiece: " + std::to_string(m.getPromotionPiece())+"\n");
    str.append("capturedPiece: " + std::to_string(m.getCapturedPiece())+"\n");
    str.append("enPassanTarget: " + std::to_string(m.getEnPassanTarget())+"\n");
    str.append("\x1b[0m");
    str.append("\n");
  }
  if(m.isKingside()) return str+"ks";
  if(m.isQueenside()) return str+"qs";
  std::string fromStr = "";
  std::string toStr = "";
  if(m.isKingside() || m.isQueenside()){
    fromStr.append("\x1b[33m");
  }
  fromStr.push_back('h'-(m.getFrom()%8));
  toStr.push_back('h'-(m.getTo()%8));
  fromStr.append(std::to_string((m.getFrom()/8)+1));
  toStr.append(std::to_string((m.getTo()/8)+1));
  
  str.append("[" + fromStr + "->" + toStr + "]");
  if(m.isPromotion()){
    switch(m.getPromotionPiece()%6){
      case BISHOP:
        str.append("b");
      break;
      case ROOK:
        str.append("r");
      break;
      case KNIGHT:
        str.append("n");
      break;
      case QUEEN:
        str.append("q");
      break;
    }
  }
  return str;
};
std::string debug::printBoard(Settings settings, Board const &board, u64 highlighted){
  std::string str = "";
  if(!board.validate()){
    str.append("\x1b[31m[error]Board is invalid\x1b[0m\n");
  }
  if(board.enPassanTarget != EN_PASSAN_NULL){
    str.append("En Passan is possible at square "+std::to_string(board.enPassanTarget)+"\n");
  }
  str.append(" a b c d e f g h\n");
  for(int file = 7; file>=0; file--){
    str.append(std::to_string(file+1));
    for(int rank = 7; rank>=0; rank--){
      if((file+rank)% 2 == 0){
        str.append(settings.lightColor);
      }else{
        str.append(settings.darkColor);
      }
      if(getBit(highlighted,getSquareIndex(file, rank))) str.append("\x1b[45m");
      std::string piece = settings.pieceCharacters[board.squares[getSquareIndex(file,rank)]];
      //std::string piece = std::to_string(board.squares[getSquareIndex(file,rank)]);
      str.append("\x1b[30m"+piece + " \x1b[0m");
    }
    str.append("\x1b[0m" + std::to_string(file+1)+ "\n");
  }
  str.append(" a b c d e f g h\n");
  return str;
};

std::string debug::printMove(Settings settings, Board const board, Move m){
  u64 highlightedSquares = (u64)0;
  setBit(highlightedSquares,m.getTo());
  setBit(highlightedSquares,m.getFrom());
  if(!(board.flags & WHITE_TO_MOVE_BIT)){
    if(m.isKingside()){
      highlightedSquares = (u64)0;
      setBit(highlightedSquares,59);
      setBit(highlightedSquares,57);
    }
    if(m.isQueenside()){
      highlightedSquares = (u64)0;
      setBit(highlightedSquares,59);
      setBit(highlightedSquares,61);
    } 
  }
  else {
    if(m.isKingside()){
      highlightedSquares = (u64)0;
      setBit(highlightedSquares,3);
      setBit(highlightedSquares,1);
    }
    if(m.isQueenside()){
      highlightedSquares = (u64)0;
      setBit(highlightedSquares,3);
      setBit(highlightedSquares,5);
    }
  }
  return printBoard(settings,board,highlightedSquares);
};

std::string debug::printBitboard(debug::Settings settings,Board board,u64 const &bb){
  return printBoard(settings, board, bb);
};
