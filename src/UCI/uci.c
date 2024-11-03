#include "uci.h"
#include "rstr.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Core/types.h"



#define MAX_TOKENS 50
struct TokenList{
  rstr tokens[MAX_TOKENS];
  int len;
};
typedef struct TokenList TokenList;

void createTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    tl->tokens[i].buf = NULL;
  }
}

void destroyTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    destroyRstr(&tl->tokens[i]);
  }
}

void tokeniseRstr(rstr *source, TokenList *tl){
  tl->len = 0;
  int tokenIndex = -1;
  bool creatingToken = false;
  for(int i = 0; i<rstrLen(source); i++){
    char c = rstrGetChar(source,i);
    if(c != ' ' && creatingToken == false){
      tokenIndex++;
      if(tl->tokens[tokenIndex].buf != NULL){
        rstrSet(&tl->tokens[tokenIndex],"");
      }else{
        tl->tokens[tokenIndex] = createRstr();
      }
      creatingToken = true;
    }
    if(creatingToken){
      if(c == ' '){
        creatingToken = false;
      }else{
        rstrAppendChar(&tl->tokens[tokenIndex],c);
      }
    }
  }
  tl->len = tokenIndex+1;
}

void rstrFromStdin(rstr *str){
  rstrSet(str,"");
  char buf[8];
  while(fgets(buf,8,stdin) != NULL){
    rstrAppend(str,buf);
    if(buf[strlen(buf)-1] == '\n') break;
  }
  rstrSetChar(str,rstrLen(str)-1,'\0');//remove newline character
}

void runUCI(){
  rstr input = createRstr();
  TokenList tl;
  createTokenList(&tl);
  bool quit = false;
  while(!quit){
    rstrFromStdin(&input);
    tokeniseRstr(&input,&tl);
    if(rstrEqual(&tl.tokens[0],"quit")){
      quit = true;
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"uci")){
      //send name and author of engine
      printf("id name "ENGINE_NAME"\n");
      printf("id author "AUTHOR"\n");

      printf("uciok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"isready")){
      printf("readyok\n");
      continue;
    }
    if(rstrEqual(&tl.tokens[0],"echo")){
      if(tl.len<2) {
        printf("not enough arguments");
        continue;
      }
      printf("%s\n", tl.tokens[1].buf);
    }
  }
  destroyTokenList(&tl);
  destroyRstr(&input);
}
