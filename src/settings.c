#include "settings.h"

#include "io/io.h"
#include "io/tokens.h"

Settings settings;

void loadSettings(){
  FILE *file = fopen("settings.txt","r");
  rstr line = createRstr();
  TokenList tl;
  createTokenList(&tl);
  int lineNumber = 1;
  bool quit = false;
  while(!quit){
    quit = rstrFromStream(&line,file);
    if(rstrGetChar(&line,0) == '\0') continue;//line is empty
    if(rstrGetChar(&line,0) == '#') continue;
    tokeniseRstr(&line,&tl);
    if(rstrEqual(&tl.tokens[0],"mode")){
      settings.mode = -1;
      if(rstrEqual(&tl.tokens[1],"cli")) settings.mode = MODE_CLI;
      if(rstrEqual(&tl.tokens[1],"uci")) settings.mode = MODE_UCI;
      if(settings.mode == -1) fprintf(stderr,"[error] invalid mode (%s) on line %d of settings.txt\n",tl.tokens[1].buf,lineNumber);
    }
    lineNumber++;
  }
  destroyRstr(&line);
}
