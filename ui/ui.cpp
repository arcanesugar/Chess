#include "ui.h"

ConsoleState consoleState;

void runConsoleInterface(Board *boardptr){
  setUnicodePieces(&consoleState.settings);
  consoleState.boardptr = boardptr;
  bool quit = false;
  while (!quit) {
    if(consoleState.printBoard) printBoard(consoleState.settings,*boardptr);
    getNextInput();
    std::string input = consoleState.lastInput;
    if(input == "mve") makeMoveFromConsole();
    if(input == "evl") consoleState.output = std::to_string(evaluate(boardptr)) + "\n";
    if(input == "bst") {
      Move best = search(*boardptr, 2);
      makeMove(boardptr,&best);
      consoleState.history.push(best);
      consoleState.printBoard = false;
      printMoveOnBoard(consoleState.settings, *boardptr, best);
    }
    if(input == "dsp")  displaySettings();
    if(input == "lgl") printLegalMoves();
    if(input == "rnd") makeRandomMove();
    if(input == "trn") whosTurnIsIt();
    if(input == "hlp" || input == "help") showHelpMenu();
    if(input == "sch") searchForMagics();
    if(input == "tst") runMoveGenerationTest(boardptr);
    if(input == "mgs") runMoveGenerationSuite();
    if(input == "und") undoLastMove(); 
    if(input == "dbg") showDebugView();
    if(input == "q" || input == "quit" || input == "exit") quit = true;

    if(input == "ks"){
      Move m;
      setSpecialMoveData(&m,CASTLE_KINGSIDE);
      makeMove(boardptr,&m);
      consoleState.history.push(m);
    }
    if(input == "qs"){
      Move m;
      setSpecialMoveData(&m,CASTLE_QUEENSIDE);
      makeMove(boardptr,&m);
      consoleState.history.push(m);
    }
  }
}

void getNextInput() {
  printf("%s>>","out");
  char temp[INPUT_MAX_LEN];
  if(scanf("%255s",temp) != 1){printf("Invalid input\n"); return;}
  if(!(strlen(temp) == 0)) {
    strcpy(consoleState.lastInput,temp);
  }
  consoleState.output = "";
  consoleState.printBoard = true;
}

void undoLastMove(){
  if(consoleState.history.empty()){
    consoleState.output = "No more move history is avalible\n\x1b[2m(If you believe this is a mistake, contact your local library)\n\x1b[0m";
    return;
  }
  Move m = consoleState.history.top();
  unmakeMove(consoleState.boardptr,&m);
  
  printMoveOnBoard(consoleState.settings,*consoleState.boardptr, m);
  consoleState.printBoard = false;
  
  consoleState.history.pop();
}
byte squareNameToIndex(std::string squareName) {
  byte squareIndex =
      ((squareName[1] - '0' - 1) * 8) + (7 - (squareName[0] - 'a'));
  return squareIndex;
}

void showHelpMenu(){
  consoleState.output.append(
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
void whosTurnIsIt(){
  if (consoleState.boardptr->flags & WHITE_TO_MOVE_BIT) {
    consoleState.output = "White to move";
  } else {
    consoleState.output = "Black to move";
  }
}
void makeRandomMove(){
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  if(legalMoves.end <= 0) {
    consoleState.output = "No Legal Moves";
    consoleState.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
//  std::cout<<debug::moveToStr(move,true)<<"\n";
  makeMove(consoleState.boardptr,&move);
//  std::cout<<debug::moveToStr(move,true)<<"\n";
  consoleState.history.push(move);
  printMoveOnBoard(consoleState.settings, *consoleState.boardptr, move);
  consoleState.printBoard = false;
}
void printLegalMoves(){
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  consoleState.output = std::to_string((int)legalMoves.end) + " moves printed\n";
  for (int i = 0; i < legalMoves.end; i++) {
    printMoveOnBoard(consoleState.settings, *consoleState.boardptr, legalMoves.moves[i]);
  }
  consoleState.printBoard = false;
}
void makeMoveFromConsole(){
  consoleState.output = "from:\n";
  getNextInput();
  int from = squareNameToIndex(consoleState.lastInput);
  consoleState.output = "to:\n";
  getNextInput();
  int to = squareNameToIndex(consoleState.lastInput);
  Move move;
  setFrom(&move,from);
  setTo(&move,to);
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  bool isLegal = false;
  MoveList variants;
  for(int i  =0; i<legalMoves.end; i++){
    if(getFrom(&move) == getFrom(&legalMoves.moves[i]) && getTo(&move) == getTo(&legalMoves.moves[i])){
      isLegal = true;
      move = legalMoves.moves[i];//assigns proporties like flags
      variants.append(move);
    }
  }
  if(isLegal){
    if(variants.end>1){
      consoleState.output = "This move has multiple variants, choose one\n";
      for(int i = 0; i<variants.end; i++){
        Board copy = *consoleState.boardptr;
        makeMove(&copy,&variants.moves[i]);
        consoleState.output.append("\n" + std::to_string(i) + ")\n");
        printBoard(consoleState.settings, copy);
      }
      getNextInput();
      move = variants.moves[std::stoi(consoleState.lastInput)];
    }
    makeMove(consoleState.boardptr,&move);
    consoleState.history.push(move);
  }else{
    consoleState.output = "This move is not legal, continue? (y/N)\n";
    getNextInput();
    if(consoleState.lastInput[0] == 'y'){
      makeMove(consoleState.boardptr,&move);
      consoleState.history.push(move);
    }
  }
  printMoveOnBoard(consoleState.settings, *consoleState.boardptr, move);
  consoleState.printBoard = false;
}

void displaySettings(){
  bool done = false;
  while(!done){
    printf("---Display Settings---\n  ");
    for(int i = 0; i<12; i++)
      printf("%s ", consoleState.settings.pieceCharacters[i]);
    printf("\n  Dark %s  \x1b[0m Light %s  \x1b[0m\n", consoleState.settings.darkColor, consoleState.settings.lightColor);
    printf("  0 - Use Unicode Pieces\n");
    printf("  1 - Use ASCII Pieces\n");
    printf("  2 - Set dark color\n");
    printf("  3 - Set light color\n");
    printf("  q - Done\n");
    getNextInput();
    if(consoleState.lastInput[0] == 'q') return;
    if(!isdigit(consoleState.lastInput[0])) continue;
    switch(std::stoi(consoleState.lastInput)){
      case 0:
        setUnicodePieces(&consoleState.settings);
      break;
      case 1:
        setASCIIPieces(&consoleState.settings);
      break;
      case 2:
        printf("Choose new dark color: \n");
        //consoleState.output.append(testFormatting(true)+"\n");
        //getNextInput();
        //consoleState.settings.darkColor = "\x1b[";
        //consoleState.settings.darkColor.append(consoleState.lastInput + "m");
      break;
      case 3:
        //consoleState.output = "Choose new light color(should start with 4 or 10): \n";
        //consoleState.output.append(testFormatting(true)+"\n");
        //getNextInput();
        //consoleState.settings.lightColor = "\x1b[";
        //consoleState.settings.lightColor.append(consoleState.lastInput + "m");
      break;
    }
  }
}


void showDebugView(){
  consoleState.printBoard = false;
  for(int i = 0; i<14; i++){
    printBitboardOnBoard(consoleState.settings, *consoleState.boardptr, consoleState.boardptr->bitboards[i]);
    printf("\n");
  }
  for(int i = 7; i>=0; i--){
    if(consoleState.boardptr->flags>>i&1){
      consoleState.output.append("1");
    }else{
      consoleState.output.append("0");
    }
  }
  consoleState.output.append("\n");
}
