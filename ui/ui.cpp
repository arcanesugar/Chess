#include "ui.h"
#include "debug.h"
#include "../Search/eval.h"
#include <string>

void ConsoleInterface::run(Board *boardptr,MoveGenerator *mgptr,Search *searchptr){
  this->boardptr = boardptr;
  this->mgptr = mgptr;
  this->searchptr = searchptr;
  bool quit = false;
  while (!quit) {
    if(c.printBoard) c.output = "\n" + debug::printBoard(c.settings,*boardptr) + c.output;
    getNextInput();
    std::string input = c.lastInput;
    if (input == "mve") makeMoveFromConsole();
    if (input == "evl") c.output = std::to_string(evaluate(*boardptr)) + "\n";
    if(input == "bst") {
      Move best = searchptr->search(*boardptr, 2);
      boardptr->makeMove(best);
      history.push(best);
      c.printBoard = false;
      c.output = debug::printMove(c.settings, *boardptr, best);
    }
    if(input == "dsp")  displaySettings();
    if (input == "lgl") printLegalMoves();
    if (input == "rnd") makeRandomMove();
    if (input == "trn") whosTurnIsIt();
    if(input == "hlp" || input == "help") showHelpMenu();
    if(input == "sch") searchForMagics();
    if(input == "tst") searchptr->runMoveGenerationTest(*boardptr);
    if(input == "mgs") searchptr->runMoveGenerationSuite();
    if(input == "und") undoLastMove(); 
    if(input == "dbg") showDebugView();
    if(input == "q" || input == "quit" || input == "exit") quit = true;

    if(input == "ks"){
      Move m;
      m.setSpecialMoveData(CASTLE_KINGSIDE);
      boardptr->makeMove(m);
      history.push(m);
    }
    if(input == "qs"){
      Move m;
      m.setSpecialMoveData(CASTLE_QUEENSIDE);
      boardptr->makeMove(m);
      history.push(m);
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

void ConsoleInterface::undoLastMove(){
  if(history.empty()){
    c.output = "No more move history is avalible\n\x1b[2m(If you believe this is a mistake, contact your local library)\n\x1b[0m";
    return;
  }
  Move m = history.top();
  boardptr->unmakeMove(m);
  
  c.output.append(debug::printMove(c.settings,*boardptr, m));
  c.printBoard = false;
  
  history.pop();
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
      + "  q - Quit\n"
      + "Note that if no command is entered, the last command given is repeated");
}
void ConsoleInterface::whosTurnIsIt(){
  if (boardptr->flags & WHITE_TO_MOVE_BIT) {
    c.output = "White to move";
  } else {
    c.output = "Black to move";
  }
}
void ConsoleInterface::makeRandomMove(){
  MoveList legalMoves;
  mgptr->generateMoves(*boardptr, legalMoves);
  if(legalMoves.end <= 0) {
    c.output = "No Legal Moves";
    c.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  std::cout<<debug::moveToStr(move,true)<<"\n";
  boardptr->makeMove(move);
  std::cout<<debug::moveToStr(move,true)<<"\n";
  history.push(move);
  c.output = debug::printMove(c.settings, *boardptr, move);
  c.printBoard = false;
}
void ConsoleInterface::printLegalMoves(){
  MoveList legalMoves;
  mgptr->generateMoves(*boardptr, legalMoves);
  c.output = std::to_string((int)legalMoves.end) + " moves printed\n";
  for (int i = 0; i < legalMoves.end; i++) {
    std::cout<<debug::printMove(c.settings, *boardptr, legalMoves.moves[i])<<std::endl;
  }
  c.printBoard = false;
}
void ConsoleInterface::makeMoveFromConsole(){
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
  mgptr->generateMoves(*boardptr, legalMoves);
  bool isLegal = false;
  MoveList variants;
  for(int i  =0; i<legalMoves.end; i++){
    if(move.getFrom() == legalMoves.moves[i].getFrom() && move.getTo() == legalMoves.moves[i].getTo()){
      isLegal = true;
      move = legalMoves.moves[i];//assigns proporties like flags
      variants.append(move);
    }
  }
  if(isLegal){
    if(variants.end>1){
      c.output = "This move has multiple variants, choose one\n";
      for(int i = 0; i<variants.end; i++){
        Board copy = *boardptr;
        copy.makeMove(variants.moves[i]);
        c.output.append("\n" + std::to_string(i) + ")\n");
        c.output.append(debug::printBoard(c.settings, copy));
      }
      getNextInput();
      move = variants.moves[std::stoi(c.lastInput)];
    }
    boardptr->makeMove(move);
    history.push(move);
  }else{
    c.output = "This move is not legal, continue? (y/N)\n";
    getNextInput();
    if(c.lastInput == "y"){
      boardptr->makeMove(move);
      history.push(move);
    }
  }
  c.output = debug::printMove(c.settings, *boardptr, move);
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
    c.output.append("  q - Done\n");
    getNextInput();
    if(c.lastInput == "q") return;
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
    }
  }
}

void ConsoleInterface::showDebugView(){
  c.printBoard = false;
  for(int i = 0; i<14; i++){
    c.output.append(debug::printBitboard(c.settings, *boardptr, boardptr->bitboards[i]));
    c.output.append("\n");
  }
  for(int i = 7; i>=0; i--){
    if(boardptr->flags>>i&1){
      c.output.append("1");
    }else{
      c.output.append("0");
    }
  }
  c.output.append("\n");
}
