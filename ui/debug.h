#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "../Board/board.h"

namespace debug{
  struct Settings{
    std::string pieceCharacters[13] = {"♙","♗","♘","♖","♕","♔","\x1b[1m♟","♝","♞","♜","♛","♚"," "};
    std::string darkColor  = "\x1b[103m";
    std::string lightColor = "\x1b[47m";
    void setASCIIPieces();
    void setUnicodePieces();
  };
  std::string testFormatting(bool highlightOnly = false);
  std::string printBoard(Settings settings, Board const &board, u64 highlighted = u64(0));

  std::string printMove(Settings settings, Board const board, Move m);
  std::string moveToStr(Move m, bool expanded = false);
  std::string printBitboard(debug::Settings settings,Board board,u64 const &bb);

  void runMoveGenerationTest();
}
