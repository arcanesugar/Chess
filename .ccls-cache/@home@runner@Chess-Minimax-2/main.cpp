#include <iostream>
#include <string>
#include <random>

#include "Board/board.h"
#include "debug.h"
#include "search.h"

void getNextInput(std::string &input, std::string &output) {
  if (output != "") {
    std::cout << output;
  }
  std::cout << ">>";
  std::string temp;
  std::getline(std::cin, temp);
  if(temp != ""){
    input = temp;
  }
}

byte squareNameToIndex(std::string squareName) {
  byte squareIndex =
      ((squareName[1] - '0' - 1) * 8) + (7 - (squareName[0] - 'a'));
  std::cout << std::to_string((int)squareIndex);
  return squareIndex;
}

void displaySettings(debug::Settings &settings,std::string &input, std::string &output){
  bool done = false;
  while(!done){
    output = "";
    output.append("---Display Settings---\n");
    output.append("  ");
    for(std::string p : settings.pieceCharacters){
      output.append(p+"\x1b[0m ");
    }
    output.append("\n  Dark: "+settings.darkColor + "  " + "\x1b[0m ");
    output.append("Light: "+settings.lightColor + "  " + "\x1b[0m");
    output.append("\n");
    output.append("  0 - Use Unicode Pieces\n");
    output.append("  1 - Use ASCII Pieces\n");
    output.append("  2 - Set dark color\n");
    output.append("  3 - Set light color\n");
    output.append("  9 - Done\n");
    getNextInput(input, output);
    if(!std::isdigit(input[0])) continue;
    switch(std::stoi(input)){
      case 0:
        settings.setUnicodePieces();
      break;
      case 1:
        settings.setASCIIPieces();
      break;
      case 2:
        output = "Choose new dark color: \n";
        output.append(debug::testFormatting(true)+"\n");
        getNextInput(input, output);
        settings.darkColor = "\x1b[";
        settings.darkColor.append(input + "m");
      break;
      case 3:
        output = "Choose new light color(should start with 4 or 10): \n";
        output.append(debug::testFormatting(true)+"\n");
        getNextInput(input, output);
        settings.lightColor = "\x1b[";
        settings.lightColor.append(input + "m");
      break;

      case 9:
        done = true;
        output = "";
      break;
    }
  }
}

int main() {
  Board board;
  board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
  bool quit = false;
  bool printBoard = true;
  std::string input = "";
  std::string output = "";
  Search search;
  search.init();
  debug::Settings printSettings;
  while (!quit) {
    if(printBoard) output = "\n" + debug::printBoard(printSettings,board) + output;
    getNextInput(input, output);
    output = "";
    printBoard = true;
    if (input == "mve") {
      output = "from:\n";
      getNextInput(input, output);
      int from = squareNameToIndex(input);
      output = "to:\n";
      getNextInput(input, output);
      int to = squareNameToIndex(input);
      Move move = {(byte)from, (byte)to};
      board.makeMove(move);
      output = debug::printMove(printSettings, board, move);
      printBoard = false;
    }

    if (input == "lgl") {

      MoveList legalMoves;
      search.generateMoves(board, legalMoves);
      output = std::to_string((int)legalMoves.end) + " moves printed\n";
      for (int i = 0; i < legalMoves.end; i++) {
        std::cout<<debug::printMove(printSettings, board, legalMoves.moves[i])<<std::endl;
      }
      printBoard = false;
    }
    if (input == "rnd"){
      MoveList legalMoves;
      search.generateMoves(board, legalMoves);
      if(legalMoves.end <= 0) {
        output = "No Legal Moves";
        printBoard = true;
        continue;
      }
      Move move = legalMoves.moves[rand()%legalMoves.end];
      board.makeMove(move);
      output = debug::printMove(printSettings, board, move);
      output.append(std::to_string(legalMoves.end));
      printBoard = false;
    }
    if (input == "trn") {
      if (board.flags & WHITE_TO_MOVE_BIT) {
        output = "White to move";
      } else {
        output = "Black to move";
      }
    }

    if(input == "hlp" || input == "help"){
      output.append(
          (std::string)"---Help---\n"
        + "  trn - White/Black\n"
        + "  mve - Make Move\n"
        + "  lgl - Legal Moves\n"
        + "  dsp - Display Settings\n"
        + "  rnd - Random Move\n"
        + "  tst - TODO: add testing\n");
    }
    if(input == "dsp"){
      displaySettings(printSettings, input, output);
    }
  }
  return 0;
}