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
    if(input == "evl") printf("%f\n",evaluate(boardptr));
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
  printf(">>");
  char temp[INPUT_MAX_LEN];
  fgets(temp,INPUT_MAX_LEN,stdin);
  if(strlen(temp) != 0) {
    temp[strlen(temp)-1] = '\0';//remove newline character
    strcpy(consoleState.lastInput,temp);
  }
  consoleState.printBoard = true;
}

void undoLastMove(){
  if(consoleState.history.empty()){
    printf("No more move history is avalible\n\x1b[2m(If you believe this is a mistake, contact your local library)\n\x1b[0m\n");
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
  printf("---Help---\n");
  printf("  mve - Make move\n");
  printf("  ks  - Castle Kingside\n");
  printf("  qs  - Castle Queenside\n");
  printf("  rnd - Random move\n");
  printf("  und - Undo last move\n");
  printf("  trn - Who's turn is it\n");
  printf("  lgl - list legal moves\n");
  printf("  dsp - Display settings\n");
  printf("  hlp/help - Show this list\n");
  printf("  q - Quit\n");

  printf("\n---Debug commands---\n");
  printf("  dbg - Debug View\n");
  printf("  sch - Search for magic numbers\n");
  printf("  tst - Run move generation test on current position\n");
  printf("  mgs - Run move generation test suite\n");
}
void whosTurnIsIt(){
  if (consoleState.boardptr->flags & WHITE_TO_MOVE_BIT) {
    printf("White to move"); return;
  }
  printf("Black to move"); return;
}
void makeRandomMove(){
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  if(legalMoves.end <= 0) {
    printf("No legal moves\n");
    consoleState.printBoard = true;
    return;
  }
  Move move = legalMoves.moves[rand()%legalMoves.end];
  makeMove(consoleState.boardptr,&move);
  consoleState.history.push(move);
  printMoveOnBoard(consoleState.settings, *consoleState.boardptr, move);
  consoleState.printBoard = false;
}
void printLegalMoves(){
  MoveList legalMoves;
  generateMoves(consoleState.boardptr, &legalMoves);
  printf("%i moves printed\n", legalMoves.end);
  for (int i = 0; i < legalMoves.end; i++) {
    printMoveOnBoard(consoleState.settings, *consoleState.boardptr, legalMoves.moves[i]);
  }
  consoleState.printBoard = false;
}
void makeMoveFromConsole(){
  printf("from:\n");
  getNextInput();
  int from = squareNameToIndex(consoleState.lastInput);
  printf("to:\n");
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
      printf("This move has multiple variants, choose one\n");
      for(int i = 0; i<variants.end; i++){
        Board copy = *consoleState.boardptr;
        makeMove(&copy,&variants.moves[i]);
        printf("\n%i\n",i);
        printBoard(consoleState.settings, copy);
      }
      getNextInput();
      move = variants.moves[std::stoi(consoleState.lastInput)];
    }
    makeMove(consoleState.boardptr,&move);
    consoleState.history.push(move);
  }else{
    printf("This move is not legal, continue? (y/N)\n");
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
      printf("1");
    }else{
      printf("0");
    }
  }
  printf("\n");
}
