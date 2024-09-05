#include "ui.h"
void getNextInput(ConsoleState &c) {
  std::cout << c.output<< ">>";
  std::string temp;
  std::getline(std::cin, temp);
  if(temp != "") c.lastInput = temp;//just keep the last input if input is blank
  c.output = "";
  c.printBoard = true;
}

byte squareNameToIndex(std::string squareName) {
  byte squareIndex =
      ((squareName[1] - '0' - 1) * 8) + (7 - (squareName[0] - 'a'));
  std::cout << std::to_string((int)squareIndex);
  return squareIndex;
}

void showHelpMenu(ConsoleState &c){
  c.output.append(
        (std::string)"---Help---\n"
      + "  trn - White/Black\n"
      + "  mve - Make Move\n"
      + "  lgl - Legal Moves\n"
      + "  dsp - Display Settings\n"
      + "  rnd - Random Move\n"
      + "  tst - TODO: add testing\n");
}
void whosTurnIsIt(Board &board, ConsoleState &c){
  if (board.flags & WHITE_TO_MOVE_BIT) {
    c.output = "White to move";
  } else {
    c.output = "Black to move";
  }
}
void makeRandomMove(Board &board, Search search,ConsoleState &c){
  MoveList legalMoves;
  search.generateMoves(board, legalMoves);
  if(legalMoves.end <= 0) {
    c.output = "No Legal Moves";
    c.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  board.makeMove(move);
  c.output = debug::printMove(c.settings, board, move);
  c.output.append(std::to_string(legalMoves.end));
  c.printBoard = false;
}
void printLegalMoves(Board &board, Search search, ConsoleState &c){
  MoveList legalMoves;
  search.generateMoves(board, legalMoves);
  c.output = std::to_string((int)legalMoves.end) + " moves printed\n";
  for (int i = 0; i < legalMoves.end; i++) {
    std::cout<<debug::printMove(c.settings, board, legalMoves.moves[i])<<std::endl;
  }
  c.printBoard = false;
}
void makeMoveFromConsole(Board &board, ConsoleState &c){
  c.output = "from:\n";
  getNextInput(c);
  int from = squareNameToIndex(c.lastInput);
  c.output = "to:\n";
  getNextInput(c);
  int to = squareNameToIndex(c.lastInput);
  Move move = {(byte)from, (byte)to};
  board.makeMove(move);
  c.output = debug::printMove(c.settings, board, move);
  c.printBoard = false;
}

void displaySettings(ConsoleState &c){
  bool done = false;
  while(!done){
    c.output = "";
    c.output.append("---Display Settings---\n");
    c.output.append("  ");
    for(std::string p : c.settings.pieceCharacters){
      c.output.append(p+"\x1b[0m ");
    }
    c.output.append("\n  Dark: "+c.settings.darkColor + "  " + "\x1b[0m ");
    c.output.append("Light: "+c.settings.lightColor + "  " + "\x1b[0m");
    c.output.append("\n");
    c.output.append("  0 - Use Unicode Pieces\n");
    c.output.append("  1 - Use ASCII Pieces\n");
    c.output.append("  2 - Set dark color\n");
    c.output.append("  3 - Set light color\n");
    c.output.append("  9 - Done\n");
    getNextInput(c);
    if(!std::isdigit(c.lastInput[0])) continue;
    switch(std::stoi(c.lastInput)){
      case 0:
        c.settings.setUnicodePieces();
      break;
      case 1:
        c.settings.setASCIIPieces();
      break;
      case 2:
        c.output = "Choose new dark color: \n";
        c.output.append(debug::testFormatting(true)+"\n");
        getNextInput(c);
        c.settings.darkColor = "\x1b[";
        c.settings.darkColor.append(c.lastInput + "m");
      break;
      case 3:
        c.output = "Choose new light color(should start with 4 or 10): \n";
        c.output.append(debug::testFormatting(true)+"\n");
        getNextInput(c);
        c.settings.lightColor = "\x1b[";
        c.settings.lightColor.append(c.lastInput + "m");
      break;

      case 9:
        done = true;
        c.output = "";
      break;
    }
  }
}