#pragma once
#include <iostream>
#include <string>

#include "board.h"

namespace debug{
  std::string testFormatting(){
    std::string str = "";
    std::string prefix[4] = {"3","4","9","10"};
    for(int p = 0; p<4; p++){
      for(int i = 0; i<8; i++){
        std::string num = prefix[p] + std::to_string(i);
        str.append("\x1b[" + num + "m" + num);
      }
      str.append("\x1b[0m\n");
    }
    return str;
  };

  std::string printBitboard(u64 bitboard){
    std::string str = "";
    //for(int i = 0; i<64; i++){
      //str.append(std::to_string(getBit(bitboard,i)));
    //}
    //str.append("\n");
    for(int y = 7; y>=0; y--){
      for(int x = 0; x<8; x++){
        if(getBit(bitboard, (y*8)+x)){
          str.append("\x1b[42m1 \x1b[0m");
        }else{
          str.append("\x1b[41m0 \x1b[0m");
        }
      }
      str.append("\n");
    }
    return str;
  };

  std::string printIndices(){
    std::string str;
    for(int y = 7; y>=0; y--){
      for(int x = 0; x<8; x++){
        if((x+y) % 2 == 0){
          str.append("\x1b[42m");
        }else{
          str.append("\x1b[47m");
        }
        if(x+(y*8) < 10){
           str.append(std::to_string(x+(y*8)) +" \x1b[0m");
        }else{
           str.append(std::to_string(x+(y*8)) +"\x1b[0m");
        }
      }
      str.append("\n");
    }
    return str;
  };

  std::string printBoard(board &b){
    std::string str;
    for(int y = 7; y>=0; y--){
      for(int x = 0; x<8; x++){
        if((x+y) % 2 == 0){
          str.append("\x1b[42m");
        }else{
          str.append("\x1b[47m");
        }
        if(b.getPiece(x+(y*8)) < 10){
           str.append(std::to_string(b.getPiece(x+(y*8))) +" \x1b[0m");
        }else{
           str.append(std::to_string(b.getPiece(x+(y*8))) +"\x1b[0m");
        }
      }
      str.append("\n");
    }
    return str;
  }

  std::string printBitboards(board &b){
    std::string str;
    std::string colorNames[2] = {"WHITE","BLACK"};
    std::string pieceNames[6] = {"PAWN","KNIGHT","BISHOP","ROOK","QUEEN","KING"};
    for(int c = 0; c<2; c++){
      for(int i = 0; i<6; i++){
        str.append("\n"+colorNames[c]+" "+pieceNames[i]+"\n");
        str.append(printBitboard(b.bitboards[i+(c*6)]));
        str.append("\n");
      }
    }
    return str;
  }
}