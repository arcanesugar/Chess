#include "ui.h"

void ConsoleInterface::run(Board &board, Search &search){
  bool quit = false;
  while (!quit) {
    if(c.printBoard) c.output = "\n" + debug::printBoard(c.settings,board) + c.output;
    getNextInput();
    std::string input = c.lastInput;
    if (input == "mve") makeMoveFromConsole(board, search);
    if(input == "dsp")  displaySettings();
    if (input == "lgl") printLegalMoves(board, search);
    if (input == "rnd") makeRandomMove(board, search);
    if (input == "trn") whosTurnIsIt(board);
    if(input == "hlp" || input == "help") showHelpMenu();
    if(input == "sch") search.searchForMagics();
    if(input == "tst") search.runMoveGenerationTest(board);
    if(input == "mgs") search.runMoveGenerationSuite();
    if(input == "und") board.unmakeMove(last);
    if(input == "dbg") showDebugView(board);
    if(input == "q") quit = true;

    if(input == "ks"){
      Move m;
      m.setSpecialMoveData(CASTLE_KINGSIDE);
      board.makeMove(m);
      last = m;
    }
    if(input == "qs"){
      Move m;
      m.setSpecialMoveData(CASTLE_QUEENSIDE);
      board.makeMove(m);
      last = m;
    }
  }
}

void ConsoleInterface::getNextInput() {
  std::cout << c.output<< ">>";
  std::string temp;
  std::getline(std::cin, temp);
  if(temp != "") c.lastInput = temp;//just keep the last input if input is blank
  c.output = "";
  c.printBoard = true;
}

byte ConsoleInterface::squareNameToIndex(std::string squareName) {
  byte squareIndex =
      ((squareName[1] - '0' - 1) * 8) + (7 - (squareName[0] - 'a'));
  return squareIndex;
}

void ConsoleInterface::showHelpMenu(){
  c.output.append(
        (std::string)"---Help---\n"
      + "  trn - Who's turn is it\n"
      + "  mve - Make move\n"
      + "  ks - Castle Kingside\n"
      + "  qs - Castle Queenside\n"
      + "  lgl - Show legal moves\n"
      + "  dsp - Display settings\n"
      + "  dbg - Debug View\n"
      + "  rnd - Random move\n"
      + "  sch - \"Search\" for magic numbers\n"
      + "  und - Undo last move\n"
      + "  hlp/help - Show this list\n"
      + "  tst - Run move generation test on current position\n"
      + "  mgs - Run move generation test suite\n"
      + "  q - Quit\n");
}
void ConsoleInterface::whosTurnIsIt(Board &board){
  if (board.flags & WHITE_TO_MOVE_BIT) {
    c.output = "White to move";
  } else {
    c.output = "Black to move";
  }
}
void ConsoleInterface::makeRandomMove(Board &board, Search &search){
  MoveList legalMoves;
  search.generateMoves(board, legalMoves);
  if(legalMoves.end <= 0) {
    c.output = "No Legal Moves";
    c.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  board.makeMove(move);
  last = move;
  c.output = debug::printMove(c.settings, board, move);
  c.printBoard = false;
}
void ConsoleInterface::printLegalMoves(Board &board, Search &search){
  MoveList legalMoves;
  search.generateMoves(board, legalMoves);
  c.output = std::to_string((int)legalMoves.end) + " moves printed\n";
  for (int i = 0; i < legalMoves.end; i++) {
    std::cout<<debug::printMove(c.settings, board, legalMoves.moves[i])<<std::endl;
  }
  c.printBoard = false;
}
void ConsoleInterface::makeMoveFromConsole(Board &board, Search &search){
  c.output = "from:\n";
  getNextInput();
  int from = squareNameToIndex(c.lastInput);
  c.output = "to:\n";
  getNextInput();
  int to = squareNameToIndex(c.lastInput);
  Move move;
  move.setFrom(from);
  move.setTo(to);
  MoveList legalMoves;
  search.generateMoves(board, legalMoves);
  bool isLegal = false;
  for(int i  =0; i<legalMoves.end; i++){
    if(move.getFrom() == legalMoves.moves[i].getFrom() && move.getTo() == legalMoves.moves[i].getTo()){
      isLegal = true;
      move = legalMoves.moves[i];//assigns proporties like flags
    }
  }
  if(isLegal){
    board.makeMove(move);
    last = move;
  }else{
    c.output = "This move is not legal, continue? (y/N)\n";
    getNextInput();
    if(c.lastInput == "y"){
      board.makeMove(move);
      last = move;
    }
  }
  c.output = debug::printMove(c.settings, board, move);
  c.printBoard = false;
}

void ConsoleInterface::displaySettings(){
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
    getNextInput();
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
        getNextInput();
        c.settings.darkColor = "\x1b[";
        c.settings.darkColor.append(c.lastInput + "m");
      break;
      case 3:
        c.output = "Choose new light color(should start with 4 or 10): \n";
        c.output.append(debug::testFormatting(true)+"\n");
        getNextInput();
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

void ConsoleInterface::showDebugView(Board &board){
  c.output.append(debug::printBoard(c.settings,board));
  for(int i = 0; i<14; i++){
    c.output.append(debug::printBitboard(board.bitboards[i]));
    c.output.append("\n");
  }
  for(int i = 7; i>=0; i--){
    if(board.flags>>i&1){
      c.output.append("1");
    }else{
      c.output.append("0");
    }
  }
  c.output.append("\n");
}